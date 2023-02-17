#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <windows.h>

#include "includes/movement.h"
#include "includes/board.h"
#include "includes/serialization.h"

bool whiteCastled = false;
bool blackCastled = false;

BYTE turnGlobal = WHITE; //who knows, maybe you want more than one player

int main(void)
{

    initPM();
    initMp();

    FILE* file = fopen("geometry.txt", "r");
    piece *** board = CreateBoard(file);

    printBoard(board);
    
    printf("onLine out: %d\n", onSameLine(1, 1));
    printf("offLine out: %d\n", onSameLine(1,3));
    printf("onDiagonal out %d\n", onDiagonal(1,2,2,3));
    printf("onDiagonal out %d\n", onDiagonal(0,0,7,7));
    printf("onDiagonal out %d\n", onDiagonal(0,0,6,7));
    printf("onDiagonal out %d\n", onDiagonal(0,0,6,7));
    printf("clearPathVertical out %d\n", clearPathVertical(board, 2, 0, 0));
    printf("clearPathVertical out %d\n", clearPathVertical(board, 0, 2, 0));
    printf("clearPathVertical out %d\n", clearPathVertical(board, 0, 7, 0));
    printf("clearPathVertical out %d\n", clearPathVertical(board, 7, 0, 0));
    printf("clearPathHorizontal out %d\n", clearPathHorizontal(board, 5, 0, 3));
    printf("clearPathHorizontal out %d\n", clearPathHorizontal(board, 0, 1, 6));
    printf("clearPathHorizontal out %d\n", clearPathHorizontal(board, 0, 5, 3));
    printf("clearPathDiagonal out %d\n", clearPathDiagonal(board, 5, 1, 3, 3));
    printf("clearPathDiagonal out %d\n", clearPathDiagonal(board, 5, 1, 4, 0));
    printf("clearPathDiagonal out %d\n", clearPathDiagonal(board, 5, 1, 6, 2));
    printf("clearPathDiagonal out %d\n", clearPathDiagonal(board, 6, 6, 7, 7));
    return 0;

}