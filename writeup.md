## Intro 

I would say as I progressed, for a very long time the goal got further away rather than closer. So eventually I tried to establish more realistic expectations of that goal and in doing that I ironically accomplished a lot more. 

## The Goal

The goal started as *make a chess bot in c++ that can play chess well*
The goal then got reworked to *make a functional base for a very good chess analysis engine in C++, and create a "proof of concept" for that system.*

I would say I was successful in accomplishing the second goal and, which is much more ambitious than I first realized.

## The Implementation 

I am just going to break down what each file does. Each file also contains comment lines containing specifics 

### `debug.cc`

basically the file implements the actual chess game against the bot. The user plays as white and the bot plays as black. The algorithm does a basic minMax with alpha-beta pruning style search asynchronously. Since the algorithm does not prune the highest level of nodes you can simply run any search (beyond depth 1 ofc) as a set of threads each performing a search of depth n-1 on each move candidate (first level).

The file also has a set of parameters such as a maximum node level that will cause it to return an error if reached... allowing the search function to return an error and minimize the search (which is the opposite of how you are *supposed* to do dynamic depth searches, but this solution is more of a workaround for now) and another to maximize the search. The search also is performed to two additional depths if those moves are captures.

The minMax function basically works by taking a reference to the board on the previous stack frame and copying it and then making the move that was also passed to it. This allows the whole search tree to avoid using heap space for everything except info it absolutely wants to make persistent. Thus the minMax is extremely fast (with added bonuses from multithreading), and in tests can crunch billions of positions relatively fast (minutes). 

if you replace the multithreading part with a single threaded implementation and uncomment the lines in teh minMax() function you can also try the system that uses the transposition table, while it does work the cache system still struggles with performance. 

in both cases there are plenty of printf() lines that more or less explain what everything does, as it is intended to be a step-style debugger for the algorithm. the code in the file also has a lot of comments, so I am not going to make pseudo code for it as I would basically just be rewriting the entire minMax() function in a near identical fashion. Everything is very abstracted so the control flow looks very similar to any pseudocode I could provide.

to be honest the results here are a bit choppy but the current algorithm does demonstrate knowledge maybe on the level of a beginner chess player, though when the search 

## pathGenTest.cc

Just tests the path generation system by allowing a player to make moves and then printing all available ones subsequently. instructions to compile are available in the source file.

## weights.h

Assigns values to pieces and positions for and provides function `getBoardWeight` return static evaluation given pointer to board to read.

## paths.h

Implements a `generateMoves()` to generate a set of moves given a position and turn.

## serialization.h

Provides functionality to read and save board. It also implements some basic maps of board ranks/files to their respective keys, which helps with I/O.

## movement.h

Chess piece movement logic

## error.h

Internal error codes... since exception handling is slow any error encountered are instead defined here and to be implemented by their relevant functions. 

## cache.h

When the same position is reached by two septate moves a `transposition` is reached, and if the chess bot has a means of persistently storing the result from the first time the position is reached it can save that result and the bot can then lookup and reuse it the next time. `cache.h` attempts to implement one using the lock-less method outlined by the Chessprogramming wiki where each cache entry is stored via a mapped Zobrist hash. A Zobrist hash is a 64 bit representation of each chess position after it was hashed with a pseudo random number generator to ensure a random distribution (as 64 bits of entropy are needed to avoid a key crash due to the sheer magnitude of chess positions). 

The cache maintains a lockless state by xoring it's public hash (key) with it's stored value to create a private "key". When performing a lookup you check if the signed key matches the saved private key. This way spaces can "get" values when they are attempting to write them without the need for a lock variable by saving the value and setting an arbitrary one to "claim" the node. Thus so long as the width of the key and the width of the hash in bits are the same you can have a shared hash table without the need for a lock variable controlling each value, in my case 64 bits. The mapping can also store a "depth" to indicate how far an entry has evaluated to.

By also performing a "hit" once the lookup is completed and updating its "recent use" counter you can also then enable a system where old entries are culled making it a proper cache. Furthermore C++ std::map's use a b/r tree which has very fast lookup and insertion times, so long as the tree is sorted by the public key value. Thus garbage collection (which will rely on age based search) will be slower. by constructing a shadow tree of references inserted by age you can implement faster garbage collection at the cost of higher memory overhead, a basic proof of concept for that can be found in the `cull` method of the `chessCache` object. so finding the right values to set is important to make sure those considerations are balanced.

the `debug.cc` program used a cache though it is slow, and still buggy in a multithread environment where it would dramatically improve performance. Though the first draft implementation can still be experimented with in single threaded mode.

the cache also makes use of the pool allocator for stl objects in `includes/lib/Allocator.h` [https://github.com/moya-lang/Allocator/](ref). since initially I experimented with a similar idea but instead implementing a persistent game tree, and automating garbage collection with smart pointers. I spent quite a lot of time on that approach and it failed dramatically. The reason being that the windows heap *really* does not like having millions of separate small entries for each node, and the overhead for storing all of that paging information becomes absurd (especially on windows where the heap is managed by the kernal and thus you must make syscalls to interact with the heap). With this in mind I decided to adopt this dependency due to it's easy usability and promise to further reduce heap overhead by allocating even more memory in advanced. it is implemented with some pretty absurd template declarations but that's just c++ I guess. 

currently I believe there is a bit too much overhead in the [] operator (lookup) and the internal cacheHit() function which can be optimized out to make the cache fast enough to even provide performance increases for single thread implementations, though that is my speculation.  

## Result Analysis

As it stands the algorithm is still not very good at chess. Often the bot also has to (after a long failed searches) revert to a search of such low depth that nothing useful is yielded. The bot can only incorporate a cache when single threaded as many bugs have not been worked out yet. However, the algorithm can attack and protect certain positions, and it can easily see and take hanging pieces. Though in more advanced positions the AI will trip on itself, not knowing when to evaluate important capture chains out to much much further depths. A human would be able to easily recognize the patterns indicating that thinking more moves ahead in that select scenario is important. Thus a human can very easily dynamically evaluate positions to a given depth and understand if a material trade will eventually win out in his favor. My bot however struggles with this notion, and is the next thing I plan on tackling. I now realize that chess engines are too often portrayed as number crunching systems that simply win out by brute-force computation power. In reality chess engines have quite a bit more "game knowledge" codified into them as they too are constrained by exponents.

The bot will often also make moves like this, wherein it does not know that developing the rook is of no value at this present moment, it simply reads from the static position value table, and since it can't see any other attacks at the moment this is it's goto position.

```
   A   B   C   D   E   F   G   H

1  WR  Wk  WB  WQ  WK  ..  ..  WR

2  ..  WP  WP  WP  ..  WP  WP  WP

3  WP  ..  ..  ..  ..  Wk  ..  ..

4  ..  ..  WB  ..  WP  ..  ..  ..

5  ..  ..  ..  ..  BP  ..  ..  ..

6  ..  ..  ..  BP  ..  ..  ..  ..

7  BP  BP  BP  Bk  ..  BP  BP  BP

8  BR  ..  BB  BQ  BK  BB  Bk  BR


   A   B   C   D   E   F   G   H

1  WR  Wk  WB  WQ  WK  ..  ..  WR

2  ..  WP  WP  WP  ..  WP  WP  WP

3  WP  ..  ..  ..  ..  Wk  ..  ..

4  ..  ..  WB  ..  WP  ..  ..  ..

5  ..  ..  ..  ..  BP  ..  ..  ..

6  ..  ..  ..  BP  ..  ..  ..  ..

7  BP  BP  BP  Bk  ..  BP  BP  BP

8  ..  BR  BB  BQ  BK  BB  Bk  BR
```

similarly the bot will make moves like this


```
8  BR  ..  BB  BQ  BK  BB  Bk  BR

   A   B   C   D   E   F   G   H

1  WR  Wk  WB  WQ  WK  ..  ..  WR

2  WP  WP  WP  ..  ..  WP  WP  WP

3  ..  ..  ..  ..  ..  ..  ..  ..

4  ..  ..  WB  BP  WP  ..  ..  ..

5  ..  ..  ..  ..  ..  ..  Wk  ..

6  ..  ..  ..  BP  ..  ..  ..  ..

7  BP  BP  BP  Bk  BK  BP  BP  BP

8  BR  ..  BB  BQ  ..  BB  Bk  BR


   A   B   C   D   E   F   G   H

1  WR  Wk  WB  WQ  WK  ..  ..  WR

2  WP  WP  WP  ..  ..  WP  WP  WP

3  ..  ..  ..  ..  ..  ..  ..  ..

4  ..  ..  WB  BP  WP  ..  ..  ..

5  ..  ..  ..  ..  ..  ..  ..  ..

6  BP  ..  ..  BP  ..  ..  ..  ..

7  ..  BP  BP  Bk  BK  Wk  BP  BP

8  BR  ..  BB  BQ  ..  BB  Bk  BR

where it will get completely lost, not knowing how to attack the bishop to leave the knight hanging.
```

### Future Considerations

I think that extendability is something often excluded from the scope of projects. However in my case I think my codebase is modular enough that it has a very very large amount of room to grow, even if some stuff does need to get reworked. The two below are what I am planning on investigating next.

ultimately implementing a more efficient order of search with alpha-beta pruning is a major source of improvement in the search time given that pruning can only occur when a better move has already been found somewhere in the tree. Thus accelerating the order at which "good" moves are generated also 

#### Iterative Deepening:

Iterative deepening is a solution where the bot searches to depth 1, caches the order of nodes it chose to play, and uses that to inform the order you should evaluate nodes when searching depth 2... etc, etc. obviously this requires some caching system to store these results persistently between results (zorbist hashing). From my understanding this dramatically improves the speed of alpha-beta.

#### priority based dynamic depth evaluations:

Basically, when evaluating each set of moves for a node it assigns a "priority" value to each node based on certain evaluations. Like if the node has a favorable PV-Move evaluation, or if the node is a capture, or if the node allows a piece to be threatened or covered. By assigning various priorities based on how "interesting" a move is according to previous evaluation and basic observations about the board we can easily search to higher dynamic depths for "good" moves. as the depth increases moves of lower priority are progressively disregarded. I think this could serve as a means of easily culling irrelevant moves that consume search time at higher depths, whereas anything that effects the board position in an immediately important way still gets tested. This is a means of further improving on iterative deepening by selectively finding moves that the previous search depth might have found non worthwhile, improving the likelihood of cache hits on successive searches. 

#### closing thoughts

Overall, I spent much more time focusing on memory, threads, and allocations, and evaluation speed thinking that it would yield better results when I should have spent more time on just focusing on pruning as many search nodes as possible. I would say that should I intend to keep with this project in the long run (which I currently do) then I will benefit much from such a focus on operation speed optimization. Though as it stands the current algorithm is not very strong and is largely propped up by very large system resource consumption. I think once I implement the several more optimizations to alpha-beta and parallel search operation speed will be a very good thing to have. Though as it stands the system resources being used are largely not directed towards required tasks, I think the capabilities of this project will plateau once it can efficiently use the speedy evaluation functions.

I feel like I really was only one round of improvements short from a very very capable system, though I really need to work on something else now. 
