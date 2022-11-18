#ifndef TREE_H
#define TREE_H

#include "config.h"

static const TreeElem_t NODE_POISON = ( TreeElem_t )0x5E32DEAD;

static const char* FileTreeDumpName = "tree_dump.html";

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

int PrintPreorderNodes( Node* node, FILE* file );

int GraphVizNodes( Node* node, FILE* dotFile, int* nodeNum );

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

FILE* TreeCreateDotDumpFile( Node* node, const char* fileName );
int   TreeGraphDump( Tree* tree );

//-----------------------------------------------------------------------------

int CreateGraphVizImg( const char* dotFileName, const char* fileName, const char* fileType );

//-----------------------------------------------------------------------------

#endif