#ifndef DIFFERENTIATOR_H 
#define DIFFERENTIATOR_H

#include "config.h"

#include "tree.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------

static const char* FileDiffDumpName = "diff_dump.html";

//-----------------------------------------------------------------------------

enum OperationTypes
{
    OP_ADD, 
    OP_SUB,
    OP_MUL,
    OP_DIV 
};

struct Operation
{
    int opType;
    const char* opStr;
};

static Operation Operations[] = 
{
    { OP_ADD, "+" },
    { OP_SUB, "-" },
    { OP_MUL, "*" },
    { OP_DIV, "/" }
};

static int NumOperations = sizeof( Operations ) / sizeof( Operation );

//-----------------------------------------------------------------------------

int LoadDiffData( Tree* tree, const char* diffData );

int GetOperationType( const char* op, 
                      Operation*  operations    = Operations, 
                      int         numOperations = NumOperations );

int PrintOperation( FILE*      file,
                    int        numOp, 
                    Operation* operations    = Operations, 
                    int        numOperations = NumOperations );

int PrintInorderNodes( Node* node, FILE* file );     

// DiffDump 
// {
int GraphVizNodes( Node* node, FILE* dotFile, int* nodeNum ); 

FILE* DiffCreateDotDumpFile( Node* node, const char* fileName );
int   DiffGraphDump( Tree* tree );
// } 

//-----------------------------------------------------------------------------

#endif
