#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdlib.h>
#include <windows.h> // for byte type

// most of this code is self explanitory
// actually I'd like to think everything 
// here is self explanitory tbh

enum pieceColor 
{
    EMPTY = 0,
    WHITE = 1,
    BLACK = 2
};

enum pieceType
{
    NONE = 0,
    BISHOP = 1,
    KNIGHT = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    PAWN = 6 // empty space
};

class piece
{
    public:

        BYTE type;
        BYTE color;

        piece () {}

        piece (BYTE Color, BYTE Type)
        {
            color = Color;
            type = Type;
        }
};

void printPiece(piece * pieceptr)
{

    if(pieceptr->color == EMPTY)
    {
        printf(" .. ");
    }

    else
    {
        switch(pieceptr->color)
        {
            case BLACK:
                printf(" B");
                break;

            case WHITE:
                printf(" W");
                break;
            
            default: // obligatory
                printf(" E");
                break;
        }
    
        switch(pieceptr->type)
        {
            case PAWN:
                printf("P ");
                break;

            case BISHOP:
                printf("B ");
                break;
            
            case KNIGHT:
                printf("k ");
                break;
            
            case ROOK:
                printf("R ");
                break;
            
            case QUEEN:
                printf("Q ");
                break;

            case KING:
                printf("K ");
                break;

            default:
                printf("E ");
        }
    
    }

}