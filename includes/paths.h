#define WIN32_LEAN_AND_MEAN
#pragma once

#include <windows.h>
#include <deque>

#include "board.h"
#include "movement.h"
#include "pieces.h" 
#include "weights.h"
#include "paths.h"

// each function here basically just works to iterate over and create a queue of valid
// moves for a piece in a given position. 

// 2s compliment abuse
#define inBounds(x, y) ((unsigned int) x < 8 && (unsigned int) y < 8) 

// store move information

class move
{   
    public:

        BYTE x1;
        BYTE x2;
        BYTE y1;
        BYTE y2;

        move() {}

        move(BYTE origX, BYTE origY, BYTE destX, BYTE destY)
        {
            x1=origX;
            x2=destX;
            y1=origY;
            y2=destY;
        }

        bool operator == (const move& other)
        {
            return x1 == other.x1 && y1 == other.y1 &&
                   x2 == other.x2 && y2 == other.y2;              
        }
        
        bool operator != (const move& other) {return !(*this == other);}

        ~move() {} // default destructor will work here
};

int totalNodes = 0; // basic debug util to give each node a unique
                    // id when testing

// debug I/O for movement

void printMove(move Move) {printf("(%d, %d)->(%d, %d)\n", Move.x1, Move.y1, Move.x2, Move.y2);}
void printMove(signed int x1, signed int y1, signed int x2, signed int y2) {printf("(%d, %d)->(%d, %d)\n", x1, y1, x2, y2);}

void printPathQueue(piece * board [][DIMENSION], std::deque<move> * moveQueue)
{

    printf("SHOWING PATH FRAME %d with %d child nodes: \n", totalNodes++, moveQueue->size());

    for(int i = 0; i < moveQueue->size(); i++) 
    {
        printf(" move: %d", i);
        printPiece(board[moveQueue->at(i).y1][moveQueue->at(i).x1]);
        printMove(moveQueue->at(i));
    }
}

// basic wrapper

void pushMove(piece * board [][DIMENSION], BYTE x1, BYTE y1, BYTE x2, BYTE y2, std::deque<move> * moveQueue) 
{
    // printMove(x1, y1, x2, y2);

    if(isPiece(board[y2][x2])) {moveQueue->push_front(move(x1,y1,x2,y2));} // captures get evalled at higher priority

    moveQueue->push_back(move(x1,y1,x2,y2));
}

// there are only 4 possible moves for a pawn so I'm just hardcoding them all (don't care about en passant yet)

signed int sides [] = {1, -1};
signed int pawnMoves [][2] = {{0,2}, {0,1}, {1,1}, {-1,1}};

void pawnPath(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, std::deque<move> * moveQueue)
{

    // printf("pawn path %d %d\n", x1, y1);

    signed int x2;
    signed int y2;
    signed int side = sides[turn - 1];

    for(int x = 0; x < 4; x++)
    {

        x2 = x1 + pawnMoves[x][0];
        y2 = y1 + pawnMoves[x][1] * side;

        if(inBounds(x2, y2) && pawnMove(turn, board, x1, y1, x2, y2)) {pushMove(board, x1, y1, x2, y2, moveQueue);}
    }
}

enum compass {north = 1, south = -1};
signed int directions [2] = {north, south};

void bishopPath(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, std::deque<move> * moveQueue)
{

    // printf("bishop path %d %d\n", x1, y1);

    int ext;
    int x2, y2;

    for (int xAxis = 0; xAxis < 2; xAxis++)
    {
        for (int yAxis = 0; yAxis < 2; yAxis++)
        {
        
            ext = 1;

            x2 = x1 + (ext * directions[xAxis]);
            y2 = y1 + (ext * directions[yAxis]);

            while(inBounds(x2, y2) && bishopMove(turn, board, x1, y1, x2, y2)) 
            {
                
                pushMove(board, x1, y1, x2, y2, moveQueue); 
                ext ++;

                if (inBounds(x1 + (ext * directions[xAxis]), y1 + (ext * directions[yAxis])))
                {
                    x2 = x1 + (ext * directions[xAxis]);
                    y2 = y1 + (ext * directions[yAxis]);
                }

                else {break;}
            } 
        }
    }
}

void rookPath(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, std::deque<move> * moveQueue)
{

    // printf("rook path %d %d\n", x1, y1);

    signed int ext;
    signed int x2, y2;

    for (int xAxis = 0; xAxis < 2; xAxis++)
    {

        ext = 1;

        x2 = x1 + (ext * directions[xAxis]);

        while(inBounds(x2, y1) && rookMove(turn, board, x1, y1, x2, y1)) 
        {
            pushMove(board, x1, y1, x2, y1, moveQueue);
            ext ++;
            if(inBounds(x1 + (ext * directions[xAxis]), y1)) {x2 = x1 + (ext * directions[xAxis]);}
            else {break;}
        }
    }
    
    for (int yAxis = 0; yAxis < 2; yAxis++)
    {

        ext = 1;

        y2 = y1 + (ext * directions[yAxis]);

        while(inBounds(x1, y2) && rookMove(turn, board, x1, y1, x1, y2)) 
        {
            pushMove(board, x1, y1, x1, y1 + (ext * directions[yAxis]), moveQueue);
            ext ++;
            if(inBounds(x1, y1 + (ext * directions[yAxis]))) {y2 = y1 + (ext * directions[yAxis]);}
            else {break;}
        }        
    }
}

// ez
void queenPath(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, std::deque<move> * moveQueue)
{
    // printf("queen path\n");
    rookPath(turn, board, x1, y1, moveQueue);
    bishopPath(turn, board, x1, y1, moveQueue);
}


signed int knightMoves [][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}, {2, 1}, {2, -1}}; // also just hardcoding these tbh

void knightPath(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, std::deque<move> * moveQueue)
{

    // printf("knight path %d %d\n", x1, y1);

    signed int x2;
    signed int y2;

    for(int x = 0; x < 8; x++)
    {

        x2 = x1 + knightMoves[x][0];
        y2 = y1 + knightMoves[x][1];

        if(inBounds(x2, y2) && knightMove(turn, board, x1, y1, x2, y2)) {pushMove(board, x1, y1, x2, y2, moveQueue);}
    }
}

signed int kingMoves [][2] = {{1, 0}, {1, 1}, {1, -1}, {0, -1}, {0, 1}, {-1, 0}, {-1, 1}, {-1, -1}};

void kingPath(BYTE turn, piece * board [][DIMENSION], BYTE x1, BYTE y1, std::deque<move> * moveQueue)
{

    // printf("kingPath %d %d\n", x1, y1);

    signed int x2;
    signed int y2;

    for(int x = 0; x < 8; x++)
    {

        x2 = x1 + kingMoves[x][0];
        y2 = y1 + kingMoves[x][1];

        if(inBounds(x2, y2) && kingMove(turn, board, x1, y1, x2, y2)) {pushMove(board, x1, y1, x2, y2, moveQueue);}
    }
}

void generateMoves(std::deque<move> * moveQueue, piece * board [][DIMENSION], BYTE turn)
{

    for (int x = 0; x < DIMENSION; x ++)
    {
        for (int y = 0; y < DIMENSION; y ++)
        {
            
            // printf("color: %d\n", board[y][x]->color);
            BYTE boardTurn = board[y][x]->color; 

            switch(board[y][x]->type)
            {
                case PAWN:
                    if(boardTurn == turn) {pawnPath(turn, board, x, y, moveQueue);}
                    break;

                case BISHOP:
                    if(boardTurn == turn) {bishopPath(turn, board, x, y, moveQueue);}
                    break;
            
                case KNIGHT:
                    if(boardTurn == turn) {knightPath(turn, board, x, y, moveQueue);}
                    break;
            
                case ROOK:
                    if(boardTurn == turn) {rookPath(turn, board, x, y, moveQueue);}
                    break;
            
                case QUEEN:
                    if(boardTurn == turn) {queenPath(turn, board, x, y, moveQueue);}
                    break;

                case KING:
                    if(boardTurn == turn) {kingPath(turn, board, x, y, moveQueue);}
                    break;

                default: break;

            }
        }
    }
}

void generateMoves(std::deque<move> * moveQueue, piece * board [][DIMENSION], BYTE turn, bool dbg)
{

    if(dbg) {printf("making path... turn==%d\nboard:\n", turn);}

    for (int x = 0; x < DIMENSION; x ++)
    {
        for (int y = 0; y < DIMENSION; y ++)
        {
            
            // printf("color: %d\n", board[y][x]->color);
            BYTE boardTurn = board[y][x]->color; 

            switch(board[y][x]->type)
            {
                case PAWN:
                    if(boardTurn == turn) {pawnPath(turn, board, x, y, moveQueue);}
                    break;

                case BISHOP:
                    if(boardTurn == turn) {bishopPath(turn, board, x, y, moveQueue);}
                    break;
            
                case KNIGHT:
                    if(boardTurn == turn) {knightPath(turn, board, x, y, moveQueue);}
                    break;
            
                case ROOK:
                    if(boardTurn == turn) {rookPath(turn, board, x, y, moveQueue);}
                    break;
            
                case QUEEN:
                    if(boardTurn == turn) {queenPath(turn, board, x, y, moveQueue);}
                    break;

                case KING:
                    if(boardTurn == turn) {kingPath(turn, board, x, y, moveQueue);}
                    break;

                default: break;

            }
        }
    }
}