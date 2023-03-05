// compile just with
// cl /std:c++20 /O2 /Oy debug.cc /link /release

#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <windows.h>
#include <map>
#include <deque>
#include <future>

#include "includes/pieces.h"
#include "includes/board.h"
#include "includes/serialization.h"
#include "includes/movement.h"
#include "includes/paths.h"
#include "includes/weights.h"
#include "includes/cache.h"

#define MIN_CACHE_DISTANCE 5
#define STATUS_RERUN_SEARCH -1

BYTE turns [] = {WHITE, BLACK};

enum minMaxPlayers {MAXER = 1, MINNER = -1};
int minMaxers [] = {MAXER, MINNER};

int MAXDEPTH = 7;          // max eval provided no other circumstance
uint64_t nodes = 0;

bool minimizedLastSearch = false;

int ENDGAME_MAXDEPTH = 20; // also for captures
int CAPTURE_CHAIN_FOLLOW = 2;
                  
int MIN_NODES = 1000000;   // for endgame processing
                           // should be set to value
                           // low enough that it
                           // will not be reached
                           // during normal
                           // evaluation
                
int MAX_NODES = 1000000000; // failsafe in the event that
                            // if search space gets too
                            // large and must me shrunk.

chessCache transpositionTable; // THE CACHE
                               // IT'S HERE
                               // AT LONG 
                               // LAST...
                               // (still does not work well)

// on depth 0 returns move index
// on all other depths returns board
// evaluation. both are represented 
// as ints.

int minMax(piece * oldBoard [][DIMENSION], move currentMove, int turnNo, int depth, signed int alpha, signed int beta)
{

    if(depth==0) {printf("minMax() to %d\n", MAXDEPTH);}

    bool claimed = false;

    signed int playerSide = minMaxers[turnNo % 2]; // both sides max
                                                   // inverted scoreboards

    // ok so basically every single board node has exactly 1
    // stack based representation of the board space... since
    // the callee's stack frame can be dereferenced despite
    // being dynamic memory (since unless something has gone
    // seriously wrong the above stack frame will remain in tact)

    bool capture = isPiece(oldBoard[currentMove.y2][currentMove.x2]); // follow all available 
                                                                      // captures at max depth
    
    bool kingCapture = oldBoard[currentMove.y2][currentMove.x2]->type == KING; 

    nodes ++;

    if(nodes > MAX_NODES) {return -1;}

    BYTE turn = turns[turnNo % 2];

    // printf("depth %d\n", depth);

    piece * board [DIMENSION][DIMENSION]; // printf("making board space\n");
    
    shallowCopyBoard(oldBoard, board);    // printf("copying board space\n");

    makeMove(board, &currentMove);        // printf("making move\n");

    if(kingCapture) {return getBoardWeight(board);} // do not need to evaluate further.

    // publicKey hash = zobristHash(board, turn);

    // cacheIndex response = transpositionTable[hash];

    // if(!getCacheError(&response.cData) && depth != 0 && depth + response.depth >= MAXDEPTH) {return ((signed int) getCacheVal(&response.cData));} // CACHE_OK

    // int dist = MAXDEPTH - depth;

    // if(dist >= MIN_CACHE_DISTANCE && dist > response.depth) {transpositionTable.claim(hash); claimed = true;} // indicating we must set the value later on.

    // printf("NODE INIT COMPLETE\n");
    // printBoard(board);

    if(depth >= MAXDEPTH)
    {
        if(!capture || depth >= MAXDEPTH + CAPTURE_CHAIN_FOLLOW || depth >= ENDGAME_MAXDEPTH) // all base cases 
        {

            int weight = getBoardWeight(board);

            /*if(claimed) 
            {
                printf("set cache val\n");
                transpositionTable.set(hash, initCacheVal((uint32_t) weight), dist);
            }*/

            return weight;
        }

        else {
            //printf("expanding capture chain (depth %d)\n", depth);
        }

    }

    std::deque<move> moveQueue; // object automatically
                                // destructed when scope
                                // exited

    if (depth == 0) {printBoard(board); generateMoves(&moveQueue, board, turn, 1);} // printf("generating moves\n");

    else {generateMoves(&moveQueue, board, turn);}

    signed int max = NEGATIVE_INFINITY;
    signed int eval = 0;

    if (depth == 0) // technically everyone is maxing since I just swap invert
                    // avoids need for extra code.
    {

        printf("MinMax choices:\n");
        printPathQueue(board, &moveQueue);

        int index = 0;

        printf("THINKING...\n");

        std::deque<std::future<int>> threadPool; 

        for(int i = 0; i < moveQueue.size(); i++) 
        {
            threadPool.push_back(std::async(minMax, board, moveQueue.at(i), turnNo + 1, depth + 1, alpha, beta));
        }

        for(int i = 0; i < moveQueue.size(); i++) 
        {
            
            eval = playerSide * threadPool.at(i).get();
            
            printf("move %d eval: %d\n", i, eval);

            if(eval > max) 
            {
                max = eval;
                index = i;
                
                if(playerSide == MAXER)
                    {
                        if(max > alpha) 
                        {
                            alpha = max;
                            if (beta <= alpha) {break;}
                        }
                    }

                else
                {

                    int betaMax = max * playerSide;

                    if(betaMax < beta) 
                    {
                        beta = betaMax;
                        if (beta <= alpha) {break;}
                    }   
                }

            }
        }


        if ((nodes < MIN_NODES && MAXDEPTH < ENDGAME_MAXDEPTH) && !minimizedLastSearch)
        {
            minimizedLastSearch = false;
            MAXDEPTH = MAXDEPTH + 2;
            printf("expanding search (DEPTH %d)\n", MAXDEPTH);
            return -1;
        }

        if(nodes > MAX_NODES) 
        {
            minimizedLastSearch = true; 
            MAXDEPTH = MAXDEPTH - 2;
            printf("minimizing search (DEPTH %d)\n", MAXDEPTH);
            return -1;
        }

        minimizedLastSearch = false;

        return index; // final return value

        // return index can be configured
        // to index of move queue to give
        // bot input.
    
    }

    while(!moveQueue.empty()) 
    {
        eval = playerSide * minMax(board, moveQueue.front(), turnNo + 1, depth + 1, alpha, beta); // recursive
                                                                                                  // downstream case
        
        // printf("eval: %d\n", eval);
        
        moveQueue.pop_front(); // remove member from queue
        
        if(eval > max) 
        {
            // printf("%d->%d\n", max, eval);
            max = eval;

            if(playerSide == MAXER)
                {
                    if(max > alpha) 
                    {
                        alpha = max;
                        if (beta <= alpha) {break;}
                    }
                }

            else
            {

                int betaMax = max * playerSide;

                if(betaMax < beta) 
                {
                    // printf("%d->%d\n", beta, betaMax);
                    beta = betaMax;
                    if (beta <= alpha) {break;}
                }   
            }

        }
    }

    /*
    if(claimed) 
    {
        transpositionTable.set(hash, initCacheVal((uint32_t) max), dist);
    }*/

    return max; // recursive upstream case

}

void main(void) 
{

    initZobristTable();

    int botMove = 0;
    int moveCount = 0;

    char moveStr[64]; // please no buffer overflow :(
    
    printf("MAX_DEPTH: %d\n", MAXDEPTH);
    printf("MIN_NODES: %d\n", MIN_NODES);
    printf("MAX_NODES: %d\n", MAX_NODES);
    printf("ENDGAME_MAXDEPTH: %d\n", ENDGAME_MAXDEPTH);
    printf("CAPTURE_CHAIN_FOLLOW: %d\n", CAPTURE_CHAIN_FOLLOW);

    initPM();
    initMp();
    
    piece * board [DIMENSION][DIMENSION];
    piece * board1 [DIMENSION][DIMENSION];
    initGame(board);
    move playerMove;

    std::deque<move> moveQueue;

    printf("SETUP DONE\n");

    for(;;)
    {

        printBoard(board);
        printf("enter move: ");
        scanf("%s", moveStr);
        printf("\n");
        
        if(moveStr[2] == '-')
        {
            // fun with pointer arithmetic

            BYTE x1, y1, x2, y2;

            x1 = moveMapX[*moveStr];
            y1 = moveMapY[moveStr[1]];
            x2 = moveMapX[moveStr[3]];
            y2 = moveMapY[moveStr[4]];
            
            playerMove = move(x1,y1,x2,y2);

            if(x1 == x2 && y1 == y2) {printf("same piece\n"); continue;}
            
            if(isOwned(turnGlobal, board[y1][x1]) && !isOwned(turnGlobal, board[y2][x2]))
            {
                printf("move (%d, %d)->(%d, %d)\n", x1, y1,x2,y2);
            
                if(!isValidMove(turnGlobal, board, x1, y1, x2, y2)) 
                {
                    printf("move against the rules\n");
                    continue;
                }

                moveCount ++;
                
                turnGlobal = turns[moveCount % 2];

                do 
                {
                    botMove = minMax(board, playerMove, moveCount, 0, NEGATIVE_INFINITY, INFINITY);
                    printf("nodes %d\n", nodes);
                    nodes = 0;
                } while(botMove == STATUS_RERUN_SEARCH);


                printf("GENERATED MOVES moves\n");
                makeMove(board, &playerMove);

                printBoard(board); generateMoves(&moveQueue, board, turnGlobal, 1);
                printPathQueue(board, &moveQueue);
                
                makeMove(board, &moveQueue.at(botMove));
                moveCount ++;
                turnGlobal = turns[moveCount % 2];
                moveQueue.clear();

            }
            else 
            {
                printf("move invalid\n");
            }
        }
        else {printf("bad input\n");} 
    }

}