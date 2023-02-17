#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <windows.h>
#include <map>

#include "includes/pieces.h"
#include "includes/board.h"
#include "includes/serialization.h"
#include "includes/movement.h"

bool whiteCastled = false;
bool blackCastled = false;

BYTE turnGlobal = WHITE; //who knows, maybe you want more than one player

int main(void)
{
    
    char move[64]; //please no buffer overflow :(

    printf("white to play\n");

    initPM();
    initMp();

    FILE* file = fopen("startingboard.txt", "r");
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
                
                if(turnGlobal == BLACK) {turnGlobal = WHITE; printf("white to play\n");}
                else {turnGlobal = BLACK; printf("black to play\n");}

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