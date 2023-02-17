#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <windows.h>
#include <map>

#include "includes/pieces.h"
#include "includes/board.h"
#include "includes/serialization.h"
#include "includes/movement.h"

BYTE turnGlobal = WHITE;

int main (void)
{
    
    initPM();
    initMp();

    FILE* file = fopen("startingboard.txt", "r");
    FILE* savefile = fopen("saveboard.txt", "w");
    piece *** board = CreateBoard(file);

    printf("isPiece out %x\n", isPiece(board[1][1]));
    printf("isPiece out %x\n", isPiece(board[7][7]));
    printf("isPiece out %x\n", isPiece(board[4][4]));
    printf("\n");
    printf("isEmpty out %x\n", isEmpty(board[1][1]));
    printf("isEmpty out %x\n", isEmpty(board[7][7]));
    printf("isEmpty out %x\n", isEmpty(board[4][4]));
    printf("\n");
    printf("isEnemy out %x\n", isEnemy(board[0][0], board[7][7]));
    printf("isEnemy out %x\n", isEnemy(board[0][0], board[0][1]));
    printf("isEnemy out %x\n", isEnemy(turnGlobal, board[7][7]));
    printf("isEnemy out %x\n", isEnemy(turnGlobal, board[0][0]));
    printf("\n");
    printf("isOwned out %d\n", isOwned(turnGlobal, board[0][0]));

}