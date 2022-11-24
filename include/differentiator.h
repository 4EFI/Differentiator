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

enum Types
{
    OP_TYPE, 
    VAL_TYPE, 
    VAR_TYPE
};

enum OperationTypes
{
    OP_ADD, 
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_DEG,
    OP_SIN
};

struct Operation
{
    int opType;
    const char* opStr;
    const char* opTexStrBegin;
    const char* opTexStrValue;
    const char* opTexStrEnd;
};

static Operation Operations[] = 
{  
    { OP_ADD, "+",   "",          " + ",       ""   },
    { OP_SUB, "-",   "",          " - ",       ""   },
    { OP_MUL, "*",   "",          " \\cdot ",  ""   },
    { OP_DIV, "/",   "\\dfrac{ ", " }{ ",      " }" },
    { OP_DEG, "^",   "",          "^{ ",       " }" },
    { OP_SIN, "sin", "",          "\\sin{ ",   " }" }
};

static int NumOperations = sizeof( Operations ) / sizeof( Operation );

static int UnaryOperations[] = 
{
    OP_SIN
};

static int NumUnaryOperations = sizeof( UnaryOperations ) / sizeof( int ); 

//-----------------------------------------------------------------------------

int LoadDiffData( Node* node, const char* diffData );

int IsUnaryOperation( int  numOp, 
                      int* unaryOperations    = UnaryOperations,
                      int  numUnaryOperations = NumUnaryOperations );

int GetOperationType( const char* op, 
                      Operation*  operations    = Operations, 
                      int         numOperations = NumOperations );

int GetIndexOperation( int        numOp, 
                       Operation* operations    = Operations, 
                       int        numOperations = NumOperations );

int PrintOperation( char*      str,
                    int        numOp, 
                    Operation* operations    = Operations, 
                    int        numOperations = NumOperations );

int PrintDiffNodeValue( char* str, Node* node );

int PrintLatexFormula( Node* node, FILE* file );  

Node* CreateNode( int type = 0, double dbl = 0, int op = 0, char* var = NULL, Node* left = NULL, Node* right = NULL, Node* node = NULL );
Node* CopyNode( Node* node );

int LinkNodeParents( Node* node, Node* parent );

Node* Differentiate( Node* node );

Node* GetSimplifiedConstantNode( Node* node ); 
Node* GetSimplifiedNeutralNode ( Node* node );

int SimplifyConstants( Node* node );
int SimplifyNeutrals ( Node* node );

int Simplify( Node* node );

// DiffDump 
// {
int GraphVizNodes( Node* node, FILE* dotFile, int* nodeNum ); 

FILE* DiffCreateDotDumpFile( Node* node, const char* fileName );
int   DiffGraphDump        ( Node* node, const char* str = NULL, ... );
// } 

//-----------------------------------------------------------------------------

#endif
