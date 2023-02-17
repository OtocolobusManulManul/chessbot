#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <windows.h>
#include <map>

#include "includes/pieces.h"
#include "includes/board.h"
#include "includes/serialization.h"
#include "includes/movement.h"

/*
* WBishop: 11
* wPawn: 16
* WKnight: 12
* WRook: 13
* WQueen: 14
* WKing: 15
* BPawn: 21
* BBishop: 26
* BKnight: 22
* BRook: 23
* BQueen: 24
* BKing: 25
* emptySq: 0
*/

int main(void)
{

    initPM();
    initMp();

    FILE* file = fopen("startingboard.txt", "r");
    FILE* savefile = fopen("saveboard.txt", "w");

    piece *** board = CreateBoard(file);
    printBoard(board);

    printf("WBishop: {%d : %c}\n", WBISHOP, pieceMap[WBISHOP]);
    printf("wPawn: {%d : %c}\n", WPAWN, pieceMap[WPAWN]);
    printf("WKnight: {%d : %c}\n", WKNIGHT, pieceMap[WKNIGHT]);
    printf("WRook: {%d : %c}\n", WROOK, pieceMap[WROOK]);
    printf("WQueen: {%d : %c}\n", WQUEEN, pieceMap[WQUEEN]);
    printf("WKing: {%d : %c}\n", WKING, pieceMap[WKING]);
    printf("BPawn: {%d : %c}\n", BPAWN, pieceMap[BPAWN]);
    printf("BBishop: {%d : %c}\n", BBISHOP, pieceMap[BBISHOP]);
    printf("BKnight: {%d : %c}\n", BKNIGHT, pieceMap[BKNIGHT]);
    printf("BRook: {%d : %c}\n", BROOK, pieceMap[BROOK]);
    printf("BQueen: {%d : %c}\n", BQUEEN, pieceMap[BQUEEN]);
    printf("BKing: {%d : %c}\n", BKING, pieceMap[BKING]);
    printf("emptySq: {%d : %c}\n", NOPIECE, pieceMap[NOPIECE]);
    printf("\n");
    printf("move {0x%x, %c}\n",moveMapX['A'], 'A');
    printf("move {0x%x, %c}\n",moveMapX['B'], 'B');
    printf("move {0x%x, %c}\n",moveMapX['C'], 'C');
    printf("move {0x%x, %c}\n",moveMapX['D'], 'D');
    printf("move {0x%x, %c}\n",moveMapX['E'], 'E');
    printf("move {0x%x, %c}\n",moveMapX['F'], 'F');
    printf("move {0x%x, %c}\n",moveMapX['G'], 'G');
    printf("move {0x%x, %c}\n",moveMapX['H'], 'H');
    printf("move {0x%x, %c}\n",moveMapY['1'], '1');
    printf("move {0x%x, %c}\n",moveMapY['2'], '2');
    printf("move {0x%x, %c}\n",moveMapY['3'], '3');
    printf("move {0x%x, %c}\n",moveMapY['4'], '4');
    printf("move {0x%x, %c}\n",moveMapY['5'], '5');
    printf("move {0x%x, %c}\n",moveMapY['6'], '6');
    printf("move {0x%x, %c}\n",moveMapY['7'], '7');
    printf("move {0x%x, %c}\n",moveMapY['8'], '8');

    serializeBoard(savefile, board); // saved board should be identical
                                     // to loaded board
    return 0;
}