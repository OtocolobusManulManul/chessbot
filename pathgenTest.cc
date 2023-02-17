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

BYTE turns [] = {WHITE, BLACK};

int main(void)
{
    
    int moveCount = 0;

    char move[64]; //please no buffer overflow :(

    printf("white to play\n");

    initPM();
    initMp();

    FILE* file = fopen("startingboard.txt", "r");
    fclose(file);

    FILE* savefile = fopen("saveboard.txt", "w");
    piece *** board = CreateBoard(file);

    for(;;)
    {

        printBoard(board);
        printf("enter move: ");
        scanf("%s", move);
        printf("\n");
        
        if(move[2] == '-')
        {
            // fun with pointer arithmetic

            BYTE x1, y1, x2, y2;

            x1 = moveMapX[*move];
            y1 = moveMapY[move[1]];
            x2 = moveMapX[move[3]];
            y2 = moveMapY[move[4]];
            
            if(x1 == x2 && y1 == y2) {printf("same piece\n"); continue;}
            
            if(isOwned(turnGlobal, board[y1][x1]) && !isOwned(turnGlobal, board[y2][x2]))
            {
                printf("move (%d, %d)->(%d, %d)\n", x1, y1,x2,y2);
            
                if(!isValidMove(turnGlobal, board, x1, y1, x2, y2)) 
                {
                    printf("move against the rules\n");
                    continue;
                }

                makeMove(board, moveMapX[*move], moveMapY[move[1]], moveMapX[move[3]],moveMapY[move[4]]);
                printBoard(board);
                
                printf("%d\n", inBounds(-1, 1));

                generateMoves(board, turnGlobal);

                moveCount ++;
                turnGlobal = turns[moveCount % 2];

            }
            else 
            {
                printf("move invalid\n");
            }
        }
        else {printf("bad input\n");} 
    }

    serializeBoard(savefile, board);
    return 0;
}

