#define WIN32_LEAN_AND_MEAN
#pragma once

#include <stdio.h>
#include <windows.h>
#include <deque>

#include "includes/pieces.h"
#include "includes/board.h"
#include "includes/serialization.h"
#include "includes/movement.h"
#include "includes/paths.h"

// debug metrics

#include <chrono>

uint64_t destructorCalls = 0;
uint64_t nodes = 0;

int treeDepth = 8;

class gameTreeNode;

bool garbageCollection = false;


class gameTreeSmartPtr
{

    gameTreeNode * nodePtr;
    
    public:

        gameTreeSmartPtr() {}

        friend bool collectGarBage (gameTreeSmartPtr * smartPtr);

        gameTreeSmartPtr(int d);
        ~gameTreeSmartPtr();
 
        gameTreeNode & operator *() {return *nodePtr;}
        gameTreeNode * operator ->() const {return nodePtr;}

        bool operator == (const gameTreeSmartPtr other) {return nodePtr == other.nodePtr;}
        bool operator != (const gameTreeSmartPtr other) {return nodePtr != other.nodePtr;}

};

class gameTreeNode 
{
    public:
        
        int data;
        std::deque<gameTreeSmartPtr> * children;
        
        gameTreeNode (int d)
        {
            data=d;
            children = new std::deque<gameTreeSmartPtr>;
        }

        ~gameTreeNode() {delete children;}

};

gameTreeSmartPtr::gameTreeSmartPtr(int d) {nodePtr = new gameTreeNode(d);}
gameTreeSmartPtr::~gameTreeSmartPtr() {if(garbageCollection) {destructorCalls ++; delete nodePtr;}}

gameTreeSmartPtr buildTree(int depth)
{
    
    nodes ++;
    
    gameTreeSmartPtr * root = new gameTreeSmartPtr(depth);
    
    if(depth==0){return NULL;}
  
    (*root)->children->push_back(buildTree(depth - 1));
    (*root)->children->push_back(buildTree(depth - 1));
      
    (*root)->children->push_back(buildTree(depth - 1));
    (*root)->children->push_back(buildTree(depth - 1));
      
    (*root)->children->push_back(buildTree(depth - 1));
    (*root)->children->push_back(buildTree(depth - 1));
      
    (*root)->children->push_back(buildTree(depth - 1));
    (*root)->children->push_back(buildTree(depth - 1));
      
    (*root)->children->push_back(buildTree(depth - 1));
    (*root)->children->push_back(buildTree(depth - 1));

    return *root;
}

void printGameTree(gameTreeSmartPtr root)
{
    if(root==NULL) {return;}

    for(int i = 0; i < root->data; i++) {printf("\t");}
    printf("%d\n", root->data);
    
    for(int i = 0; i < root->children->size(); i++) {printGameTree(root->children->at(i));}

}

void test ()
{
    gameTreeSmartPtr root = buildTree(7);
    garbageCollection = true;
}
