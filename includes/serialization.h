#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <map>

#include "board.h"
#include "pieces.h"

// essentially just setting up enum types for piece serializing
// stole this method from libcurls curl.h file
// but made it more efficient
// https://github.com/curl/curl/blob/master/include/curl/curl.h

#define PIECE(t, nu) t + nu * 10

enum pieceMap {

    WBISHOP = PIECE(BISHOP, WHITE),
    WPAWN = PIECE(PAWN, WHITE),
    WKNIGHT = PIECE(KNIGHT, WHITE),
    WROOK = PIECE(ROOK, WHITE),
    WQUEEN = PIECE(QUEEN, WHITE),
    WKING = PIECE(KING, WHITE),
    BBISHOP = PIECE(BISHOP, BLACK),
    BPAWN = PIECE(PAWN, BLACK),
    BKNIGHT = PIECE(KNIGHT, BLACK),
    BROOK = PIECE(ROOK, BLACK),
    BQUEEN = PIECE(QUEEN, BLACK),
    BKING = PIECE(KING, BLACK),
    NOPIECE = PIECE(NONE, EMPTY)
};

std::map<int, char> pieceMap;

void initPM ()
{
    pieceMap.insert(std::pair<int, char>(WBISHOP, 'b'));
    pieceMap.insert(std::pair<int, char>(WPAWN, 'p'));
    pieceMap.insert(std::pair<int, char>(WKNIGHT, 'n'));
    pieceMap.insert(std::pair<int, char>(WROOK, 'r'));
    pieceMap.insert(std::pair<int, char>(WQUEEN, 'q'));
    pieceMap.insert(std::pair<int, char>(WKING, 'k'));
    pieceMap.insert(std::pair<int, char>(BBISHOP, 'B'));
    pieceMap.insert(std::pair<int, char>(BPAWN, 'P'));
    pieceMap.insert(std::pair<int, char>(BKNIGHT, 'N'));
    pieceMap.insert(std::pair<int, char>(BROOK, 'R'));
    pieceMap.insert(std::pair<int, char>(BQUEEN, 'Q'));
    pieceMap.insert(std::pair<int, char>(BKING, 'K'));
    pieceMap.insert(std::pair<int, char>(NOPIECE, 'e'));
}

std::map<int, char> moveMapX;
std::map<int, char> moveMapY;

void initMp ()
{
    moveMapX.insert(std::pair<char, int>('A',0));
    moveMapX.insert(std::pair<char, int>('B',1));
    moveMapX.insert(std::pair<char, int>('C',2));
    moveMapX.insert(std::pair<char, int>('D',3));
    moveMapX.insert(std::pair<char, int>('E',4));
    moveMapX.insert(std::pair<char, int>('F',5));
    moveMapX.insert(std::pair<char, int>('G',6));
    moveMapX.insert(std::pair<char, int>('H',7));
    moveMapY.insert(std::pair<char, int>('1',0));
    moveMapY.insert(std::pair<char, int>('2',1));
    moveMapY.insert(std::pair<char, int>('3',2));
    moveMapY.insert(std::pair<char, int>('4',3));
    moveMapY.insert(std::pair<char, int>('5',4));
    moveMapY.insert(std::pair<char, int>('6',5));
    moveMapY.insert(std::pair<char, int>('7',6));
    moveMapY.insert(std::pair<char, int>('8',7));
    moveMapX.insert(std::pair<char, int>('a',0));
    moveMapX.insert(std::pair<char, int>('b',1));
    moveMapX.insert(std::pair<char, int>('b',2));
    moveMapX.insert(std::pair<char, int>('d',3));
    moveMapX.insert(std::pair<char, int>('e',4));
    moveMapX.insert(std::pair<char, int>('f',5));
    moveMapX.insert(std::pair<char, int>('g',6));
    moveMapX.insert(std::pair<char, int>('h',7));
}

piece * deserializePiece (FILE * fp)
{
    BYTE c = getc(fp);

    switch(c)
    {
        case 'e':
            return emptySq;
        case 'p':
            return wPawn;
        case 'r':
            return WRook;
        case 'n':
            return WKnight;
        case 'b':
            return WBishop;
        case 'q':
            return WQueen;
        case 'k':
            return WKing;
        case 'P':
            return BPawn;
        case 'R':
            return BRook;
        case 'N':
            return BKnight;
        case 'B':
            return BBishop;
        case 'Q':
            return BQueen;
        case 'K':
            return BKing;
        default:
            printf("ERROR while parsing\n");
            abort();
    }
}

int serializeBoard(FILE * fp, piece *** board) 
{

    for (int h = 0; h < DIMENSION; h++)
    {
    
        for (int w = 0; w < DIMENSION; w++)
        {
            BYTE color = board[h][w]->color;
            BYTE type = board[h][w]->type;
            fputc(pieceMap[color * 10 + type], fp);
        }
    }

    return fclose(fp);
}