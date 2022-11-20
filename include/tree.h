#ifndef TREE_H
#define TREE_H

#include "config.h"

#include <stdio.h>

static const TreeElem_t NODE_POISON = ( TreeElem_t )0x5E32DEAD;

enum NodeSides
{
    LEFT_SIDE  = -1,
    RIGHT_SIDE =  1
};

//-----------------------------------------------------------------------------

struct Node
{
    Node* parent;

    int side;
    
    TreeElem_t value;

    Node* left;
    Node* right;
};

int NodeCtor( Node* node );
int NodeDtor( Node* node );

//-----------------------------------------------------------------------------

struct Tree
{
    Node headNode;

    bool isEmpty;
};

int TreeCtor( Tree* tree );
int TreeDtor( Tree* tree );

Node* TreeSetNodeValue( Node* node, TreeElem_t val );
Node* TreeAddChild    ( Node* node, TreeElem_t val, int side );

Node* TreeSearch( Node* nodeBegin, TreeElem_t val );

//-----------------------------------------------------------------------------

int CreateGraphVizImg( const char* dotFileName, const char* fileName, const char* fileType );

//-----------------------------------------------------------------------------

#endif