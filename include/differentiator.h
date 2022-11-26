#ifndef DIFFERENTIATOR_H 
#define DIFFERENTIATOR_H

#include "config.h"

#include "tree.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------

static const char* FileDiffDumpName = "diff_dump.html";

static const double POISON_DBL = 0xBAADF00D32DEAD32;

//-----------------------------------------------------------------------------

enum FormulaType
{
    COMMON,
    LATEX   
};

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
    OP_SIN,
    OP_COS,
    OP_LN
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
    { OP_DIV, "/",   "\\frac{ ", " }{ ",       " }" },
    { OP_DEG, "^",   "",          "^{ ",       " }" },
    { OP_SIN, "sin", "",          "\\sin{ ",   " }" },
    { OP_COS, "cos", "",          "\\cos{ ",   " }" },
    { OP_LN,  "ln",  "",          "\\ln{ ",    " }" },
};

static int NumOperations = sizeof( Operations ) / sizeof( Operation );

static int UnaryOperations[] = 
{
    OP_SIN,
    OP_COS,
    OP_LN
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

int PrintFormula( Node* node, FILE* file = stdout, int formulaType = FormulaType::COMMON );

Node* CreateNode( int type = 0, double dbl = 0, int op = 0, char* var = NULL, Node* left = NULL, Node* right = NULL, Node* parent = NULL );
Node* CopyNode( Node* node );
int   SetNodeParent( Node* node, Node* parent );
int   ReplaceNode  ( Node* node, Node* newNode );

int LinkNodeParents( Node* node, Node* parent );

Node* Differentiate( Node* node, const char* varName = "x" );

int IsVarInTree( Node* node, const char* varName = "x" );

Node* GetSimplifiedConstantNode( Node* node, const char* varName = NULL, double val = 0 ); 
Node* GetSimplifiedNeutralNode ( Node* node );

int SimplifyConstants( Node* node, const char* varName = NULL, double val = 0 );
int SimplifyNeutrals ( Node* node );

int Simplify( Node* node );

Node* CalcValueAtPoint( Node* node, const char* varName, double val, double* answer = NULL );

// Latex
// {
int PrintBeginTex   ( FILE* texFile );

int IncludeImgToTex ( const char* imgName,     FILE* fileName, double scale = 1 );
int CreateTexFile   ( const char* texFileName, Node* node );
int CreatePdfFromTex( const char* texFileName );
// }

int CreateFuncGraphImg( Node* node, const char* imgName, double xMin, double xMax, const char* varName = "x" );

Node* ExpandIntoTaylorSeries( Node* node, int n, double x_0 = 0 );

uint64_t Factorial( uint64_t num );

// DiffDump 
// {
int GraphVizNodes( Node* node, FILE* dotFile, int* nodeNum ); 

FILE* DiffCreateDotDumpFile( Node* node, const char* fileName );
int   DiffGraphDump        ( Node* node, const char* str = NULL, ... );
// } 

//-----------------------------------------------------------------------------

#endif
