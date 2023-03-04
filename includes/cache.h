// in memory-transposition table

// writing this make me a much better
// C++ programmer

// based on
// https://www.chessprogramming.org/Shared_Hash_Table#Lock-less 
// and
// https://www.chessprogramming.org/Zobrist_Hashing

// the primitives behind the hashing
// logic are not meant to be secure... the point
// is not to hide data, but rather prevent cache
// access errors 

// file is less stylistically consistant most the rest of
// this is because the transpo-table needs to be much more
// abstracted than the rest of the primitive types/methods
// of the project because it's a lot more complicated.

// this cache is very "loose"
// it will go over it's limit
// and some values will not be
// precise due to cache hit errors
// in parallel envs
// though none should cause major
// issues. as the margin for error
// is built in.

#define WIN32_LEAN_AND_MEAN
#pragma once

#include <map>
#include <windows.h>
#include <random>
#include <thread>

// #include <winnt.h>

#include "lib/Allocator.h"

#include "board.h"
#include "pieces.h"
#include "error.h"

// MB alignment since it makes
// it more human readable

#define CACHE_MAX_MB 4000 // max cache size in megabytes 
uint64_t cacheMax = 1000000 * CACHE_MAX_MB;

uint64_t cacheAge = 0; // given that 4gb (the arbitrary max cache size I picked)
                       // is literally the full virtual address space of a 32 bit 
                       // process this kinda needs to be a 64 bit int.

typedef DWORD64 PUBLIC_KEY; // need to separate key types
                            // one with "age" index
                            // allowing for map to sort
                            // another without
                            // since actual game does not
                            // need to initialize a key age
                            // for one just used to search
                            // the transposition table

__int64 salt = 0xdeadbeefdadf00d;

typedef DWORD64 POSITION_HASH;
typedef uint64_t  CACHE_DATA; // bottom 32 bits is error code
                              // top is weight.
                              // plenty more room to shove
                              // extra info in if need be.

typedef DWORD64 publicKey;

// I initially typdef'd this class as a primitive (see above)
// and I still would like to treat it like one
// though that also means I need to overload
// a lot of operators (so the c++ compiler
// can inline all of them at runtime)
 
class CACHE_KEY
{
    public:
        
        int age; // for std::map sorting
        publicKey key;

        CACHE_KEY() {age = cacheAge++;} // for default initializer
 
        CACHE_KEY(publicKey pubKey) {key = pubKey; age = cacheAge++;}
 
        // THESE COMPARE AGE OF KEY FOR std::map

        bool operator == (const CACHE_KEY other) {return key == other.key;} 

        bool operator == (const publicKey other) {return (DWORD64)key == (DWORD64)other;} // C syntax so much less horrible here

        CACHE_KEY& operator ^= (const publicKey other) 
        {
            key ^= other; // the "C way" works much better here
            return *this;
        }

        CACHE_KEY& operator = (publicKey pubKey) // assumes hash has already been randomized 
        {
            key = pubKey;  
            // printf("assigning key 0x%p, 0x%p\n", pubKey, key);
            return *this;
        }

};

// api to get and set cache values.

void setCacheError(CACHE_DATA * cd, uint32_t err)
{
    uint32_t* bottomPtr = (reinterpret_cast<uint32_t*>(cd)) + 1;
    *bottomPtr = err;
}

void setCacheVal(CACHE_DATA * cd, uint32_t weight) 
{
    setCacheError(cd, CACHE_OK);
    uint32_t* topPtr = reinterpret_cast<uint32_t*>(cd);
    *topPtr = weight;
}

CACHE_DATA initCacheVal(uint32_t val)
{
    // return ((uint64_t) val << 32) | NULL;
    uint32_t cacheVal = static_cast<uint32_t>(val);
    return static_cast<uint64_t>(cacheVal);
}

CACHE_DATA initCacheError(uint32_t err) 
{
    return ((uint64_t) err << 32) | NULL;
}

uint32_t getCacheError(CACHE_DATA * cd) {return *((reinterpret_cast<uint32_t*>(cd)) + 1);}

int getCacheVal(CACHE_DATA * cd) {return *(reinterpret_cast<int*>(cd));}

std::map<POSITION_HASH, publicKey> ZOBRIST_PRNG_TABLE;   // technically this is all randomized by ASLR
                                                         // since each piece is just a pointer to heap
                                                         // space. such an approach is horribly insecure 
                                                         // since it can lead to address space derandomization
                                                         // but this is a chess engine... who cares


// non psuedorandom representation of each board state
POSITION_HASH hashPosition(piece * boardPiece, int x, int y, BYTE turn)
{
    uintptr_t pieceVal = reinterpret_cast<uintptr_t>(boardPiece); // winternals trick
    POSITION_HASH hash = (POSITION_HASH) pieceVal; 
    
    return hash + (x * 0x100000000000) + (y * 0x10000000000000) + (turn * 0x1000000000000000); // one byte for x, 
                                                                                               // one for y, 
                                                                                               // one free.
                                                                                               // rest is ptr
                                                                                               // plenty of space
                                                                                               // if I need to cram
                                                                                               // data later
}

// pseudorandom number generator

publicKey xorShift (publicKey pos)
{
    pos ^= pos << 31;
    pos ^= pos >> 17;
    pos ^= pos << 5;
    pos ^= pos << 23;
    pos ^= pos << 8;
    pos ^= pos << 12;

    pos += (publicKey) salt; // adding an additional random number
                             // lowers key collision likelihood

    return pos; 
}

// iterate over every single piece type
// for every board square and generate
// a seed number 

void initZobristTable() 
{

    // stack allocated since it will only be needed
    // once

    # define PIECE_COUNT 13

    piece * pieces[PIECE_COUNT] = {
        wPawn,
        WBishop,
        WKnight,
        WRook,
        WQueen,
        WKing,
        BPawn,
        BBishop,
        BKnight,
        BRook,
        BQueen,
        BKing,
        emptySq
    };

    for(int turn = WHITE; turn <= BLACK; turn++)
    {
        for(int i = 0; i < PIECE_COUNT; i++)
        {
            for(int x = 0; x < DIMENSION; x++)
            {
                for(int y = 0; y < DIMENSION; y++)
                {

                    POSITION_HASH pos = (publicKey) hashPosition(pieces[i], x, y, turn);
                    publicKey key = xorShift(pos);
                    ZOBRIST_PRNG_TABLE.insert(std::pair<POSITION_HASH, publicKey>(pos, key));

                    // printf("pos: 0x%p,\tkey: 0x%p\n", pos, key);

                    // KVPs are inverted here.
                    // since we are looking up
                    // the key for each position
                    // that can be serialized to
                    // cache

                }
            }
        }
    }
}

// https://www.chessprogramming.org/Zobrist_Hashing

publicKey zobristHash(piece * board [][DIMENSION], BYTE turn)
{
    publicKey key = NULL; // MUST SET VALUE TO ZERO BEFORE MAKING
                          // HASH DUE TO RISK OF UNINITIALIZED
                          // MEMORY GETTING XORSHIFTED

    for(int x = 0; x < DIMENSION; x++)
    {
        for(int y = 0; y < DIMENSION; y++) 
        {
            // printf("local key: 0x%p\n",  ZOBRIST_PRNG_TABLE[hashPosition(board[y][x], x, y, turn)]);
            // printf("key 0x%p\n", key);
            key ^= ZOBRIST_PRNG_TABLE[hashPosition(board[y][x], x, y, turn)];
        }
    }

    // printf("zobristHash() return key 0x%p\n", key);
    return key;
}

// https://www.chessprogramming.org/Shared_Hash_Table#Lock-less
// "hits" not currently utilized at the moment
// but may be required for other replacement policies

class CACHE_ENTRY // the ""back back" end
{

    public:

        int depth;

        void print()
        {
            printf("depth: %d data: 0x%p privKey 0x%p\n", depth, data, key);
        }

        CACHE_ENTRY(CACHE_DATA cData, publicKey pubKey, int evalDepth)
        {
            depth = evalDepth;
            data = cData;
            key = pubKey ^ cData;
        }

        CACHE_ENTRY(CACHE_DATA cData, publicKey pubKey)
        {
            depth = 0;
            data = cData;
            key = pubKey ^ cData;
        }

        void reloc() {setCacheError(&data, CACHE_RELOC);} // cache being relocated
                                                          // new entry will have data soon

        ~CACHE_ENTRY() {}

        CACHE_DATA lookup (publicKey pubKey) // read but don't write data
        {
            if(key ^ data == pubKey) {hits ++; return data;} // hits tracked at return statement
                                                                    // since code might repeatably try 
                                                                    // to access busy cache entry
            return CACHE_MISSMATCH;
        }

        CACHE_DATA cacheGet (publicKey publicKey)
        {
            CACHE_DATA cData = this->lookup(publicKey);

            if(cData != CACHE_MISSMATCH)
            {
                data = CACHE_BUSY; // set the "lock"
                return cData;
            }

            return CACHE_MISSMATCH;
        }

        // you need to pass the public key back since the context *should*
        // have it on hand, and passing it saves the entry the time required to
        // recalculate it.

        // any code that calls cacheGet() must call cacheSet() (or higher level bindings) 
        // afterwards since otherwise you'd leave it inaccessible 

        CACHE_DATA cacheSet (publicKey oldKey, publicKey publicKey, CACHE_DATA newData)
        {
            if(oldKey == key) // if old private key matches
            {
                data = newData;
                key = publicKey ^ data;
                return newData;
            }

            cMsg = CACHE_BAD_HANDSHAKE; // we really don't want this to happen
            return CACHE_BAD_HANDSHAKE;
        }

    private:

        int hits; // not needed so far, but could be relevant for further debugging
        CACHE_DATA data;
        publicKey key; // paradoxical yes... but better here than on the frontend.
};

// std::map orders
// on increasing order
// so to invert that
// we need to pass this
// overloaded operator to
// it

// printf("CACHE_KEY_CMP() a: %p b: %p\n", a.key, b.key);

template <>
struct std::less<CACHE_KEY>
{
    bool operator()(const CACHE_KEY &a, const CACHE_KEY &b) const {return b.key < a.key;}
};

// making these would suck so much if
// compiler didn't do it for you by 
// throwing error messages.

typedef Moya::Allocator<std::map<CACHE_KEY, CACHE_ENTRY>::value_type, 4096> MapMemoryPoolAllocator;
typedef std::map<CACHE_KEY, CACHE_ENTRY, std::less<CACHE_KEY>, MapMemoryPoolAllocator> cacheContainer; // we need to add a reference to std::less()
                                                                                                       // in order to help the allocator order the 
                                                                                                       // tree

                                                                                                       // basically this allows for an associative
                                                                                                       // queue, which is what we want

typedef std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const CACHE_KEY,CACHE_ENTRY>>>> cacheEntry;

typedef std::pair<const CACHE_KEY,CACHE_ENTRY> entryRef; // for garbage collection

class cacheIndex
{
    public:
        int depth;
        CACHE_DATA cData;

        cacheIndex() {}

        cacheIndex(CACHE_DATA data, int cDepth)
        {
            depth = cDepth;
            cData = data;
        }

        cacheIndex(CACHE_DATA err)
        {
            cData = err;
            depth = NULL;
        }

};

// use LRU replacement policy with a block queue to reduce overhead
// and make thread safety easier.

class chessCache
{
    private: // the "backend"
        
        cacheContainer container;
        bool culling = false; // is a garbage collection thread currently active ?? 

        void cachePush(publicKey pubKey, CACHE_ENTRY newData) // cData contains the private key 
        {

            CACHE_KEY key;
            key = pubKey;

            std::pair<CACHE_KEY, CACHE_ENTRY> pair = std::pair<CACHE_KEY, CACHE_ENTRY>(key, newData);
            container.emplace_hint(container.begin(), pair);
        }

        void cachePush(publicKey pubKey, CACHE_DATA cData) // cData contains the private key 
        {

            // printf("pushing pubKey 0x%p\n", pubKey);

            CACHE_KEY key;
            key = pubKey;

            CACHE_ENTRY newData(cData, pubKey);
            std::pair<CACHE_KEY, CACHE_ENTRY> pair = std::pair<CACHE_KEY, CACHE_ENTRY>(key, newData);
            
            // printf("emplacing 0x%p\n", cData);
            
            container.emplace_hint(container.begin(), pair);
        }

        void cacheHit (cacheEntry it, publicKey pubKey)
        {
            CACHE_ENTRY entry = it->second; // we aren't changing the data
            it->second.reloc();

            container.erase(it);
            cachePush(pubKey, entry);
        }

        // since std::map is ordered we want to keep the most recently used cache
        // lines to occupy the beginning to reduce search time
        // so less used 
        // https://stackoverflow.com/questions/6438086/iterator-invalidation-rules-for-c-containers/6438087#6438087

        void cull(int entries) // culling should be roughly nlogn
        {

            culling = true; printf("CULLING...\n");

            std::map<int, entryRef> deathRow;
            
            for(entryRef& ref : container)
            {
                deathRow.emplace(ref.first.age, ref);
            }
            
            int count = 0;

            for (auto& it : deathRow) // I don't know if there is a way to reduce this complexity further
                                      // by directly supplying refs. but I don't think that will work.
            {
                container.erase(container.find(it.second.first));
                count++;

                if(count >= entries) {deathRow.clear(); break;}
            
            }
            
            printf("DONE CULLING %d...\n", entries);
            culling = false;
        
        }

    public:

        int maxSize;

        void debugCull(int entries) {cull(entries);}

        chessCache(uint64_t max) {maxSize = max;}
        chessCache() {maxSize = cacheMax;}

        ~chessCache() {container.clear();}

        void printCache () 
        {
            printf("showing full cache\n");
            for(auto& x:container) //{cout<<x.first<<" "<<x.second.first<<" "<<x.second.second<<endl;}
            {
                printf("pubKey: 0x%p, age: %d ", x.first.key, x.first.age);
                x.second.print();
            }
            printf("END.\n");
        }

        cacheIndex operator [] (publicKey pubKey)
        {

            // printf("pubkey           query\n");
            auto it = container.find(pubKey);    
            // printf("SEARCH OVER\n");

            // printf("key 0x%p NOT FOUND\n", pubKey);
            if(it == container.end()) 
            {
                return cacheIndex(initCacheError(CACHE_NOT_FOUND));
            }

            CACHE_DATA data = it->second.lookup(pubKey);
            cacheHit(it, pubKey); // if found

            if(container.size() * (sizeof(CACHE_KEY) + sizeof(CACHE_ENTRY)) >= maxSize && !culling)
            {
                std::thread garbageCollector([this] {cull(40960);});   // culling overhead high O(1)
                                                                       // need to cull a lot to be
                                                                       // efficient.
            }

            return cacheIndex(data, it->second.depth);
        }

        // for when a new node exists and is being evaluated by a running
        // context... assumes the node's nonexistence is verified.

        void claim (publicKey pubKey) 
        {
            // printf("claiming pubKey 0x%p\n", pubKey);
            cachePush(pubKey, initCacheError(CACHE_CLAIMED));
        }

        // assumes the existence of entry 
        // is already verified
        
        CACHE_DATA get(publicKey pubKey) 
        {
            auto it = container.find(pubKey); // wonders of modern c++
            return it->second.cacheGet(pubKey);
        }

        void set(publicKey pubKey, CACHE_DATA newData, int depth) // only set if depth of
                                                                  // evaluation is higher
                                                                  // than if previously set
        {
            auto it = container.find(pubKey);
            
            if(depth > it->second.depth)
            {
                container.erase(it);

                CACHE_ENTRY data(newData, pubKey, depth);

                cachePush(pubKey, data);
            }
        }
};