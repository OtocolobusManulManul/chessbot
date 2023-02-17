// board movement primitives
// each function name is self
// explanitory

#define WIN32_LEAN_AND_MEAN
#pragma once

#include "pieces.h"


bool whiteCastled = false;
bool blackCastled = false;

bool wRook1Moved = false;
bool wRook2Moved = false;
bool bRook1Moved = false;
bool bRook2Moved = false;

#define inBounds(x, y) ((unsigned int) x < 8 && (unsigned int) y < 8)

#define DIMENSION 8 // it doesn't compile unless you redefine this macro
                    // idk why and I don't care
                    // just don't delete it pls

BYTE turnGlobal = WHITE; //who knows, maybe you want more than one player

bool isPiece(piece * piece) {return piece->color != EMPTY;}
bool isEmpty(piece * piece) {return piece->color == EMPTY;}
bool isOwned(BYTE turn, piece * piece1) {return piece1->color == turn;}
bool onSameLine (BYTE p1, BYTE p2) {return p1 == p2;}
bool onDiagonal (BYTE x1, BYTE y1, BYTE x2, BYTE y2) {return abs(y2 - y1) == abs(x2 - x1);}

bool isEnemy(piece * piece1, piece * piece2) {return piece1->color != piece2->color;}
bool isEnemy(BYTE turn, piece * piece2) {return turn != piece2->color && piece2->color != EMPTY;}

bool clearPathHorizontal(piece * board [][DIMENSION], BYTE p1, BYTE p2, BYTE y) 
{
    BYTE direction = 1;
    if(p1 > p2) {direction = -1;}   
    
    if(isOwned(board[y][p1]->color, board[y][p2])) {return false;}

    for(BYTE n = p1 + direction; n != p2 && inBounds(n, n); n = n + direction) 
    {
        if(!isEmpty(board[y][n])) {return false;}
    }

    return true;

}

bool clearPathVertical(piece * board [][DIMENSION], BYTE p1, BYTE p2, BYTE x) 
{

    BYTE direction = 1;
    if(p1 > p2) {direction = -1;}    

    if(isOwned(board[p1][x]->color, board[p2][x])) {return false;}

    for(BYTE n = p1 + direction; n != p2 && n > 0 && n < 8; n = n + direction) 
    {
        if(!isEmpty(board[n][x])) {return false;}
    }

    return true;

}

bool clearPathDiagonal(piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2)
{

    if(isOwned(board[y1][x1]->color, board[y2][x2])) {return false;}

    BYTE yDir = -1;
    BYTE xDir = -1;

    if(x2 > x1) {xDir = 1;}
    if(y2 > y1) {yDir = 1;}

    x1 += xDir;
    y1 += yDir;

    while(x1 != x2)
    {

        if(isPiece(board[y1][x1])) {return false;}

        x1 += xDir;
        y1 += yDir;

    }

    return true;
}

#define BPAWNROW 6
#define WPAWNROW 1

bool pawnMove(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2) // AHHHHHHHHHHHHHHHHH
{

    bool firstMove = false;

    if(turn == BLACK && y1 == BPAWNROW) {firstMove = true;}

    if(turn == BLACK)
    {
        if(x1 == x2)
        {
            if(y1 - y2 == 1 && isEmpty(board[y2][x2]))
            {
                return true;
            }
            
            else if(firstMove)
            {
                if(y1 - y2 == 2 && isEmpty(board[y2 + 1][x2]) && isEmpty(board[y2][x2])) 
                {
                    return true;
                }
            }
        }

        if(y1 - y2 == 1 && abs(x1-x2) == 1 && isEnemy(turn, board[y2][x2]))
        {
            return true;
        }
        
        return false;
    }
    
    if(turn == WHITE && y1 == WPAWNROW) {firstMove = true;}

    if(turn == WHITE)
    {
        if(x1 == x2)
        {

            if(y2 - y1 == 1 && isEmpty(board[y2][x2]))
            {
                return true;
            }
            
            else if(firstMove)
            {
                if(y2 - y1 == 2 && isEmpty(board[y2 - 1][x2]) && isEmpty(board[y2][x2])) {return true;}
            }
        }

        if(y2 - y1 == 1 && abs(x1-x2) == 1 && isEnemy(turn, board[y2][x2])) {return true;}
        return false;
    }

}

bool bishopMove(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2)
{

    if(onDiagonal(x1, y1, x2, y2))
    {
        if(clearPathDiagonal(board, x1, y1, x2, y2)) {return true;}
    } 

    return false;
}

bool knightMove(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2)
{
    if((abs(x1-x2) == 2 && abs(y1-y2) == 1) || abs(x1-x2) == 1 && abs(y1-y2) == 2) {return !isOwned(turn, board[y2][x2]);}
    return false;
}

bool rookMove(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2)
{

    // lambda checks if the rooks have moved 
    // yet and sets global vars accordingly
    // once more saved the need for a lot of
    // copypaste code

    auto _rookFirstMove = [=](BYTE x, BYTE y) -> void
    {
        #define checkR(in, x, y, x1, y1) if (!in && x1 == x && y1 == y) {in = !in;}

        checkR(wRook1Moved, 0, 0, x1, y1)
        checkR(wRook2Moved, 7, 0, x1, y1)
        checkR(bRook1Moved, 0, 7, x1, y1)
        checkR(bRook2Moved, 7, 7, x1, y1)
    };

    if(onSameLine(x1, x2)) 
    {
        bool ret = clearPathVertical(board, y1, y2, x1);
        _rookFirstMove(x1, y1);
        return ret;
    }

    if(onSameLine(y1, y2))
    {
        bool ret = clearPathHorizontal(board, x1, x2, y1);
        _rookFirstMove(x1, y1);
        return ret;
    }
    
    return false;
}

bool kingMove(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2) 
{
    if(abs(x1-x2) <= 1 && abs(y1-y2) <= 1) 
    {
        return !isOwned(turn, board[y2][x2]);
    }

    return false;
}

bool queenMove(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2) {return rookMove(turn, board, x1, y1, x2, y2) || bishopMove(turn, board, x1, y1, x2, y2);}

bool isValidMove(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2) 
{

    piece * pieceptr = board[y1][x1];

    switch(pieceptr->type)
        {
            case PAWN:
                return pawnMove(turn, board, x1, y1, x2, y2);
                break;

            case BISHOP:
                return bishopMove(turn, board, x1, y1, x2, y2);
                break;
            
            case KNIGHT:
                return knightMove(turn, board, x1, y1, x2, y2);
                break;
            
            case ROOK:
                return rookMove(turn, board, x1, y1, x2, y2);
                break;
            
            case QUEEN:
                return queenMove(turn, board, x1, y1, x2, y2);
                break;

            case KING:
                return kingMove(turn, board, x1, y1, x2, y2);
                break;
        }

        return true;
}