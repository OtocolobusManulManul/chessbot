// compile just with
// cl debug.cc /link /release /O2 /Oy

#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <windows.h>
#include <map>

#include "includes/pieces.h"
#include "includes/board.h"
#include "includes/serialization.h"
#include "includes/movement.h"
#include "includes/paths.h"
#include "includes/weights.h"

#define STATUS_RERUN_SEARCH -1

BYTE turns [] = {WHITE, BLACK};

enum minMaxPlayers {MAXER = 1, MINNER = -1};
int minMaxers [] = {MAXER, MINNER};

int MAXDEPTH = 7;          // max eval provided no other circumstance
int nodes = 0;

int ENDGAME_MAXDEPTH = 20; // also for captures
int CAPTURE_CHAIN_FOLLOW = 2;

int MIN_NODES = 10000000;  // for endgame processing
                           // should be set to value
                           // low enough that it
                           // will not be reached
                           // during normal
                           // evaluation
         
int MAX_NODES = 500000000; // failsafe in the event that
                           // if search space gets too
                           // large and must me shrunk.

// polymorphic minmax (sorta)
// on depth 0 returns move index
// on all other depths returns board
// evaluation. both are represented 
// as ints.

int minMax(piece * oldBoard [][DIMENSION], move currentMove, int turnNo, int depth, signed int alpha, signed int beta)
{

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

    // printf("NODE INIT COMPLETE\n");
    // printBoard(board);

    if(depth >= MAXDEPTH)
    {
        if(!capture || depth >= MAXDEPTH + CAPTURE_CHAIN_FOLLOW || depth >= ENDGAME_MAXDEPTH) // all base cases 
        {
            return getBoardWeight(board);
        }

        else {
            //printf("expanding capture chain (depth %d)\n", depth);
        }

    }

    std::deque<move> moveQueue; // object automatically
                                // destructed when scope
                                // exited

    generateMoves(&moveQueue, board, turn); // printf("generating moves\n");

    signed int playerSide = minMaxers[turnNo % 2]; // both sides max
                                                   // inverted scoreboards

    signed int max = NEGATIVE_INFINITY;
    signed int eval = 0;

    if (depth == 0) // technically everyone is maxing since I just swap invert
                    // avoids need for extra code.
    {

        printPathQueue(board, &moveQueue);

        int index = 0;

        printf("THINKING...\n");

        for(int i = 0; i < moveQueue.size(); i++) 
        {
            
            eval = playerSide * minMax(board, moveQueue.at(i), turnNo + 1, depth + 1, alpha, beta);
            
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


        if (nodes < MIN_NODES && MAXDEPTH < ENDGAME_MAXDEPTH)
        {
            printf("expanding search\n");
            MAXDEPTH++;
            return -1;
        }

        if(nodes > MAX_NODES) 
        {
            printf("minimizing search\n");
            MAXDEPTH;
            return -1;
        }

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

    return max; // recursive upstream case

}

void main(void) 
{

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
                generateMoves(&moveQueue, board, turnGlobal);
                
                do 
                {
                    botMove = minMax(board, playerMove, moveCount, 0, NEGATIVE_INFINITY, INFINITY);
                } while(botMove == STATUS_RERUN_SEARCH);

                printf("nodes %d\n", nodes);
                nodes = 0;

                printPathQueue(board, &moveQueue);
                printf("BOT MOVE: %d\n", botMove);

                makeMove(board, &playerMove);
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