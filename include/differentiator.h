#ifndef DIFFERENTIATOR_H 
#define DIFFERENTIATOR_H

#include "config.h"

#include "tree.h"
#include "stack.h"
#include "str_algs.h"

#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------

static const char* FileDiffDumpName = "diff_dump.html";

static const double POISON_DBL = 0xBAADF00D32DEAD32;

//-----------------------------------------------------------------------------

static const char* LinkExprs[] = 
{
    "Ну я думаю, если с пивком посидеть подумать, то получается следующее",
    "Вот только скажите, что не очевидно следующее выражение",
    "Все уже поняли, что",
    "Моя мама сказала, что будет так, значит будет так",
    "Следующее выражение предлагаю получить вам самостоятельно",
    "Следующее выражение остается как упражнение",
    "Сам М-А-А-А-А-К-Р-О-С получил следующее"
};

const int NumLinkExprs = sizeof( LinkExprs ) / sizeof( char* );

struct VarValue
{
    const char* var;
    double      value;
};

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
    OP_ADD = 0, 
    OP_SUB = 1,

    OP_MUL = 3,
    OP_DIV = 4,

    OP_LN  = 6,

    OP_SIN = 8,
    OP_COS = 9,

    OP_DEG = 11,

    OP_O
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
    { OP_O,   "o",   "",          "o{ ",       " }" }
};

static int NumOperations = sizeof( Operations ) / sizeof( Operation );

static int UnaryOperations[] = 
{
    OP_SIN,
    OP_COS,
    OP_LN,
    OP_O
};

static int NumUnaryOperations = sizeof( UnaryOperations ) / sizeof( int ); 

//-----------------------------------------------------------------------------

int LoadDiffDataTree( const char* diffDataTree, Node* node );
int LoadDiffDataNums( String* diffDataStrs, int numStrs, int* nDiff, char* varName, int* nTaylor );

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
Node* CopyNode     ( const Node* node );
int   SetNodeParent(       Node* node, Node* parent );
int   ReplaceNode  (       Node* node, Node* newNode );

int LinkNodeParents( Node* node, Node* parent );

Node* Differentiate ( const Node* node, const char* varName = "x" );
Node* DifferentiateN( const Node* node, const char* varName, int n, FILE* file = NULL );

int IsVarInTree( const Node* node, const char* varName = "x" );

Node* GetSimplifiedConstantNode( Node* node);
Node* GetSimplifiedNeutralNode ( Node* node );

int SimplifyConstants( Node* node );
int SimplifyNeutrals ( Node* node );

int Simplify( Node* node );

Node* FuncSubstituteVarValues( const Node* node, const char*    varName,       double val );
Node* FuncSubstituteVarValues( const Node* node, const VarValue arrVarValue[], int    num );

Node* CalcValueAtPoint( const Node* node, const VarValue arrVarValue[], int    num, double* answer = NULL );
Node* CalcValueAtPoint( const Node* node, const char*    varName,       double val, double* answer = NULL );

double CalcErrorAtPoint( const Node* node, const VarValue arrVarValue[], const VarValue errors[], int num, Node** errorNode = NULL );

// Latex
// {
int PrintBeginTex   ( FILE* texFile );

int IncludeImgToTex  ( const char* imgName,     FILE* fileName, double scale = 1 );
int CreateDiffTexFile( const char* texFileName, Node* node, int nDiff, const char* varName, int nTaylor );
int CreatePdfFromTex ( const char* texFileName );
// }

int CreateFuncGraphImg( Node* node, const char* imgName, double xMin, double xMax, const char* varName = "x" );

Node* ExpandIntoTaylorSeries( Node* node, const char* varName, int n, double x_0 = 0 );

uint64_t Factorial( uint64_t num );

Node* GetTangentEquationAtPoint( Node* node, const char* varName, double val );

// DiffDump 
// {
int GraphVizNodes( Node* node, FILE* dotFile, int* nodeNum ); 

FILE* DiffCreateDotDumpFile( Node* node, const char* fileName );
int   DiffGraphDump        ( Node* node, const char* str = NULL, ... );
// } 

int Rand( int minVal, int maxVal );

//-----------------------------------------------------------------------------

#endif
