#define WIN32_LEAN_AND_MEAN
#pragma once

#include <climits>

#include "board.h"
#include "paths.h"

#ifdef INFINITY
#undef INFINITY
#endif

#define INFINITY 1000000
#define NEGATIVE_INFINITY -1000000 // because of 2s compliment negative bias

#define DIMENSION 8 

enum pieceValues
{
    pawnVal = 20,
    knightVal = 60,
    bishopVal = 60,
    rookVal = 100,
    queenVal = 180,
    kingVal = INFINITY,
};

// the below are weighted tables for each piece on each position 

signed int kingWeightMap [DIMENSION][DIMENSION] = 
{
    {-6, -8, -8, -10, -10, -8, -8 , -6},
    {-6, -8, -8, -10, -10, -8, -8 , -6},
    {-6, -8, -8, -10, -10, -8, -8 , -6},
    {-6, -8, -8, -10, -10, -8, -8 , -6},
    {-4, -6, -6, -8, -8, -6, -6, -4},
    {-2, -4, -4, -4, -4, -4, -4, -2},
    {4, 4, 0, 0, 0, 0, 4, 4},
    {4, 6, 2, 0, 0, 2, 6, 4}
};

signed int queenWeightMap [DIMENSION][DIMENSION] =
{
    {-4, -2, -2, -1, -1, -2, -2, -4},
    {-2, 0, 0, 0, 0, 0, 0, -2},
    {-2, 0, 1, 1, 1, 1, 0, -2},
    {-1, 0, 1, 1, 1, 1, 0, -1},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {-2, 1, 1, 1, 1, 1, 0, -2},
    {-2, 0, 1, 0, 0, 0, 0, -2},
    {-4, -2, -2, -1, -1, -2, -2, -4}
};

signed int rookWeightMap [DIMENSION][DIMENSION] =
{
    {0, 0, 0, 1, 1, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 1},
    {-1, 0, 0, 0, 0, 0, 0, -1},
    {-1, 0, 0, 0, 0, 0, 0, -1},
    {-1, 0, 0, 0, 0, 0, 0, -1},
    {-1, 0, 0, 0, 0, 0, 0, -1},
    {-1, 0, 0, 0, 0, 0, 0, -1},
    {0, 0, 0, 1, 1, 0, 0, 0}
};

signed int bishopWeightMap [DIMENSION][DIMENSION] =
{
    {-4, -2, -2, -2, -2, -2, -2, -4},
    {-2, 0, 0, 0, 0, 0, 0, -2},
    {-2, 0, 1, 2, 2, 1, 0, -2},
    {-2, 1, 1, 2, 2, 1, 0, -2},
    {-2, 0, 2, 2, 2, 2, 0, -2},
    {-2, 2, 2, 2, 2, 2, 2, -2},
    {-2, 1, 0, 0, 0, 0, 1, -2},
    {-4, -2, -2, -2, -2, -2, -2, -4}
};

signed int knightWeightMap [DIMENSION][DIMENSION] =
{
    {-10, -8, -6, -6, -6, -6, -8, -10},
    {-8, -4, 0, 0, 0, 0, -4, -8},
    {-6, 0, 2, 3, 3, 3, 2, 0 -6},
    {-6, 1, 3, 4, 4, 3, 1, -6},
    {-6, 0, 3, 4, 4, 3, 0, -6},
    {-6, 1, 2, 3, 3, 2, 1, -6},
    {-8, -4, 0, 1, 1, 0, -4, -8},
    {-10, -8, -6, -6, -6, -6, -8, -10}
};

signed int pawnWeightMap [DIMENSION][DIMENSION] =
{
    {0, 0, 0, 0, 0, 0, 0, 0},
    {10, 10, 10, 10, 10, 10, 10, 10},
    {3, 3, 5, 8, 7, 5, 3, 3},
    {2, 2, 3, 6, 6, 3, 2, 2},
    {0, 0, 0, 4, 4, 0, 0, 0},
    {1, -1, -2, 0, 0, -2, -1, 1},
    {1, 2, 2, -4, -4, 2, 2, 1},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

// debug function 

void printWeights(int boardWeightMap[][DIMENSION], BYTE turn)
{

    if(turn == WHITE)
    {    
        for (int x = 0; x < DIMENSION; x ++ )
        {
            for (int y = 0; y < DIMENSION; y ++ ) {printf("%03d ", boardWeightMap[x][y]);}
            printf("\n\n");
        }
    }

    else
    {
        for (int x = 0; x < DIMENSION; x ++ )
        {
            for (int y = 0; y < DIMENSION; y ++ ) {printf("%03d ", boardWeightMap[x][y]);}
            printf("\n\n");
        }
    }

    printf("\n");

}

// grab weight of space

signed int getSpaceWeight (piece * board [][DIMENSION], BYTE x, BYTE y)
{

    piece * pieceptr = board[x][y];

    enum sides {whiteSide = 1, blackSide = -1};
    
    // lambda expression just returning
    // the weight given coords and color
    // and piece (saves a lot of copy
    // pasting on my part).

    auto _spaceWeight = [](signed int boardWeightMap[][DIMENSION], signed int side, BYTE x, BYTE y) -> signed int
    {
        if(side == whiteSide) {return boardWeightMap[x][y];} 
        return boardWeightMap[(DIMENSION - 1) - x][(DIMENSION - 1) - y] * side;
    };

    signed int side = 0;

    switch(pieceptr->color)
    {
        case BLACK:
            side = whiteSide;
            break;

        case WHITE:
            side = blackSide; 
            break;
            
        default: // obligatory
            return 0;
            break;
    }
    
    switch(pieceptr->type)
    {
        case PAWN:
            return _spaceWeight(pawnWeightMap, side, x, y) + pawnVal * side;
            break;

        case BISHOP:
            return _spaceWeight(bishopWeightMap, side, x, y) + bishopVal * side;
            break;
            
        case KNIGHT:
            return _spaceWeight(knightWeightMap, side, x, y) + knightVal * side;
            break;
            
        case ROOK:
            return _spaceWeight(rookWeightMap, side, x, y) + rookVal * side;
            break;
            
        case QUEEN:
            return _spaceWeight(queenWeightMap, side, x, y) + queenVal * side;
            break;

        case KING:
            return _spaceWeight(kingWeightMap, side, x, y) + kingVal * side;
            break;

        default:
            return 0;
    }
    
}

// gets weight of full board position

signed int getBoardWeight (piece * board [][DIMENSION])
{

    int weight = 0;

    for (int x = 0; x < DIMENSION; x ++ )
    {
        for (int y = 0; y < DIMENSION; y ++ ) {weight += getSpaceWeight(board, x, y);}    
    }

    return weight;
}
