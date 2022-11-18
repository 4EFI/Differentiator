#ifndef DIFFERENTIATOR_H 
#define DIFFERENTIATOR_H

#include "config.h"

#include "tree.h"
#include "stack.h"

#include  <stdarg.h>

//-----------------------------------------------------------------------------

struct DiffNode
{
    enum 
    { 
        OP_TYPE, 
        OP_VAL, 
        OP_VAR 
    } 
    type; 

    // union in progress
    double dblValue;
    enum 
    {
        OP_ADD, 
        OP_SUB,
        OP_MUL,
        OP_DIV 
    } 
    opValue;
    const char* varValue;
};


//-----------------------------------------------------------------------------

int SaveDiffData( Node* node, const char* fileName );
int LoadDiffData( Node* node, FILE* file, int side = 0 );

int CreatePath( Tree* tree, Node* node, Stack* stk );

//-----------------------------------------------------------------------------

#endif
