
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>

#include "differentiator.h"
#include "tree.h"
#include "LOG.h"
#include "my_assert.h"
#include "stack.h"
#include "file_algs.h"
#include "str_algs.h"
#include "LOG.h"
#include "dsl.h"

//-----------------------------------------------------------------------------

FILE* FileDiffDump = fopen( FileDiffDumpName, "w" );

//-----------------------------------------------------------------------------

int SetDiffNode( Node* node, const char* diffData, int* numOp )
{
    ASSERT( node     != NULL, 0 );
    ASSERT( diffData != NULL, 0 );
    
    int numReadSyms = 0;

    double num = 0;
    int isNum = sscanf( diffData, " %lf%n ", &num, &numReadSyms );

    if( isNum )
    {        
        node->value->type     = Types::VAL_TYPE;
        node->value->dblValue = num;
    }
    else
    {
        char* tempStr = ( char* )calloc( MaxStrLen, sizeof( char ) );

        sscanf( diffData, " %[^ ()]%n ", tempStr, &numReadSyms );

        int opType = GetOperationType( tempStr );
        (*numOp)   = opType; 

        if( opType == -1 )
        {
            node->value->type     = Types::VAR_TYPE;
            node->value->varValue = tempStr;    
        }
        else
        {  
            node->value->type    = Types::OP_TYPE;
            node->value->opValue = opType;   
        }
    }

    return numReadSyms; 
}

int LoadDiffDataTree( const char* diffData, Node* node ) 
{
    ASSERT( node     != NULL, 0 );
    ASSERT( diffData != NULL, 0 );

    Node* currNode = node;
    int   currOp   = -1;

    int len = strlen( diffData );
    for( int i = 0; i < len; i++ )
    {
        if( diffData[i] == ' ' ) continue;
        
        if ( diffData[i] == '(' )
        {
            if( !currNode->left && IsUnaryOperation( currOp ) == -1 )
            {
                currNode = TreeAddChild( currNode, NULL, LEFT_SIDE );
                continue;
            }
            else
            {
                currNode = TreeAddChild( currNode, NULL, RIGHT_SIDE );
                continue;
            }
        }

        if( diffData[i] == ')' )
        {
            currNode = currNode->parent;
            continue;
        }

        i += SetDiffNode( currNode, diffData + i, &currOp );
        i --;
    }

    LinkNodeParents( node, NULL );

    return 1;
}

int LoadDiffDataNums( String* diffDataStrs, int numStrs, int* nDiff, char* varName, int* nTaylor )
{
    ASSERT( diffDataStrs != NULL, 0 );

    // n diff
    sscanf( diffDataStrs[2].str, "%d", nDiff );
    // var name
    sscanf( diffDataStrs[4].str, "%s", varName );
    // n Taylor
    sscanf( diffDataStrs[6].str, "%d", nTaylor );

    return 1;
}

//-----------------------------------------------------------------------------

int IsUnaryOperation( int numOp, int* unaryOpearations, int numUnaryOperations )
{
    ASSERT( unaryOpearations != NULL, 0 );

    for( int i = 0; i < numUnaryOperations; i++ )
    {
        if( unaryOpearations[i] == numOp ) return i;
    }

    return -1;
}

//-----------------------------------------------------------------------------

int GetOperationType( const char* opStr, Operation* operations, int numOperations )
{
    ASSERT( opStr      != NULL, 0 );
    ASSERT( operations != NULL, 0 );

    for( int i = 0; i < numOperations; i++ )
    {
        if( stricmp( opStr, operations[i].opStr ) == 0 )
        {
            return operations[i].opType;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------

int PrintOperation( char* str, int numOp, Operation* operations, int numOperations )
{
    ASSERT( str        != NULL, 0 );
    ASSERT( operations != NULL, 0 );

    int indOp = GetIndexOperation( numOp, operations, numOperations );     
    if( indOp == -1 ) return -1; 
    
    sprintf( str, " %s ", operations[ indOp ].opStr );
            
    return 1;
}

//-----------------------------------------------------------------------------

int GetIndexOperation( int numOp, Operation* operations, int numOperations )
{
    ASSERT( operations != NULL, 0 );

    for( int i = 0; i < numOperations; i++ )
    {
        if( operations[i].opType == numOp ) return i;
    }

    return -1;
}

//-----------------------------------------------------------------------------

#define PrintTex( pos )                                             \
{                                                                   \
    if( node->value->type == Types::OP_TYPE )                       \
    {                                                               \
        int indOp = GetIndexOperation( node->value->opValue );      \
        if( indOp == -1 ) return -1;                                \
                                                                    \
        fprintf( file, "%s", (Operations[ indOp ]).opTexStr##pos ); \
    }                                                               \
}

int PrintFormulaRecursively( Node* node, FILE* file, int formulaType ) 
{
    ASSERT( node != NULL, 0 );
    ASSERT( file != NULL, 0 );

    int isLatex = ( formulaType == FormulaType::LATEX ? 1 : 0 );

    if( node->left && node->right && node->parent != NULL )
    {
        fprintf( file, "( " );  
    }
    
    if( isLatex ) PrintTex( Begin );

    if( node->left )  
    {
        PrintFormulaRecursively( node->left, file, formulaType );
    }
    
    if( !isLatex || node->value->type != OP_TYPE )
    {
        char strValue[ MaxStrLen ] = "";
        PrintDiffNodeValue( strValue, node );

        fprintf( file, "%s", strValue );
    }
    else
    {
        PrintTex( Value );
    }
    
    if( node->right ) 
    {
        PrintFormulaRecursively( node->right, file, formulaType );
    }

    if( isLatex ) PrintTex( End );

    if( node->left && node->right && node->parent != NULL )
    {
        fprintf( file, " )" );  
    }

    return 1;
}

int PrintFormula( Node* node, FILE* file, int formulaType )
{
    ASSERT( node != NULL, 0 );
    ASSERT( file != NULL, 0 );
    
    PrintFormulaRecursively( node, file, formulaType );

    return 1;
}

//-----------------------------------------------------------------------------

int PrintDiffNodeValue( char* str, Node* node )
{
    ASSERT( str  != NULL, 0 );
    ASSERT( node != NULL, 0 );

    if( !node->value )
    {
        sprintf( str, "" );
        return 0;
    }
    
    if/* */( node->value->type == VAL_TYPE )
    {
        sprintf( str, "%g", node->value->dblValue );
        return VAL_TYPE;
    }
    else if( node->value->type == OP_TYPE )
    {
        PrintOperation( str, node->value->opValue );
        return OP_TYPE;
    }
    else if( node->value->type == VAR_TYPE )
    {
        sprintf( str, "%s", node->value->varValue );
        return VAR_TYPE;
    }
    
    return 1;
}

//-----------------------------------------------------------------------------

Node* CreateNode( int type, double dbl, int op, char* var, Node* left, Node* right, Node* parent )
{    
    Node* newNode = ( Node* )calloc( 1, sizeof( Node ) );

    newNode->value           = ( DiffNode* )calloc( 1, sizeof( DiffNode ) );    
    newNode->value->type     = type;
    newNode->value->dblValue = dbl;
    newNode->value->opValue  = op;
    newNode->value->varValue = var;

    newNode->parent = parent;
    newNode->left   = left;
    newNode->right  = right;

    return newNode;
}

Node* CopyNode( Node* node )
{
    ASSERT( node != NULL, 0 );

    Node* newNode = ( Node* )calloc( 1, sizeof( Node ) );

    newNode->value = ( DiffNode* )calloc( 1, sizeof( DiffNode ) );

    memmove( newNode,        node,        sizeof(     Node ) );
    memmove( newNode->value, node->value, sizeof( DiffNode ) );

    if( node->left )
    {
        newNode->left = CopyNode( node->left );
        newNode->left->parent = newNode; 
    }

    if( node->right )
    {
        newNode->right = CopyNode( node->right );
        newNode->right->parent = newNode; 
    }

    return newNode;
}

// Set only at one node
int SetNodeParent( Node* node, Node* parent )
{
    if( node == NULL ) return 0;

    node->parent = parent;

    return 1;
}

// Replace left/right node to newNode from previous 
int ReplaceNode( Node* node, Node* newNode )
{
    ASSERT( node != NULL, 0 );

    newNode->parent = node->parent;
        
    if/* */( IS_L ) { node->parent->left  = newNode; }
    else if( IS_R ) { node->parent->right = newNode; }

    if( node->parent == NULL ) 
    {
        memmove( node, newNode, sizeof( Node ) );
        
        SetNodeParent( node->left,  node );
        SetNodeParent( node->right, node );
    }

    return 1;
}

//-----------------------------------------------------------------------------


// Differentiate
//-----------------------------------------------------------------------------

Node* DifferentiateNode( Node* node, const char* varName )
{
    ASSERT( node        != NULL, 0 );
    ASSERT( node->value != NULL, 0 );

    switch( node->value->type )
    {
        case VAL_TYPE:
            return CREATE_VAL_NODE( 0 );

        case VAR_TYPE:
            if( strcmp( node->value->varValue, varName) == 0 ) 
            {
                return CREATE_VAL_NODE( 1 );
            }
            else
            {
                return CREATE_VAL_NODE( 0 );
            }

        case OP_TYPE:
            switch( node->value->opValue )
            {
                case OP_ADD:
                    return ADD( DL, DR );

                case OP_SUB:
                    return SUB( DL, DR );
                    
                case OP_MUL:
                    return ADD(  MUL( DL, CR ), MUL( CL, DR )  );

                case OP_DIV:
                    return DIV(  SUB( MUL( DL, CR ), MUL( CL, DR ) ), POW( CR, CREATE_VAL_NODE(2) )  );

                case OP_DEG:
                {
                    int isVarInLeft  = IsVarInTree( node->left,  varName );
					int isVarInRight = IsVarInTree( node->right, varName );
					
					if/* */( isVarInLeft && isVarInRight )
					{
						return MUL(  POW( CL, CR ), ADD( MUL( DR, LN( NULL, CR ) ), MUL(CR, MUL( DL, DIV( CREATE_VAL_NODE(1), CL ) ) ) )  ); 
					}
					else if( isVarInLeft )
					{
						return MUL(  MUL( CR, POW( CL, SUB( CR, CREATE_VAL_NODE(1) ) ) ), DL  );
					}
					else if( isVarInRight )
					{
						return MUL(  MUL( LN( NULL, CL ), POW( CL, CR ) ), DR  );
					}
					else
                    {
						return CREATE_VAL_NODE( 1 ); 
                    }

					break;
                }

                case OP_SIN:
                    return MUL(  COS( NULL, CR ), DR  );

                case OP_COS:
                    return MUL(  MUL( SIN( NULL, CR ), CREATE_VAL_NODE(-1) ), DR  );

                case OP_LN:
                    return MUL(  DIV( CREATE_VAL_NODE(1), CR ), DR  );

                default:
                    return node;
            }
        default:
            break;
    }

    return node;
}

int IsVarInTree( Node* node, const char* varName )
{
    ASSERT( node    != NULL, 0 );
    ASSERT( varName != NULL, 0 )
	
	if( node->value->type == VAR_TYPE && !strcmp( node->value->varValue, varName ) )
    {
		return 1;
    }
	if( !node->left && !node->right )
    {
		return 0;
    }

	if( IsVarInTree( node->left,  varName ) ) return 1;
	
    if( IsVarInTree( node->right, varName ) ) return 1;

	return 0;
}

// All nodes from headNode
int LinkNodeParents( Node* node, Node* parent )
{
    ASSERT( node != NULL, 0 );

    node->parent = parent;

    if( node->left )
    {
        LinkNodeParents( node->left, node );
    }
    
    if( node->right )
    {
        LinkNodeParents( node->right, node );
    }

    return 1; 
}

Node* Differentiate( Node* node, const char* varName )
{
    ASSERT( node != NULL, 0 );

    Node* newNode = node;

    newNode = DifferentiateNode( newNode, varName );
    LinkNodeParents( newNode, NULL );

    return newNode;
}

Node* DifferentiateN( Node* node, const char* varName, const char* linkExprs[], int numExps, FILE* file )
{
    ASSERT( node    != NULL, 0 );
    ASSERT( varName != NULL, 0 );

    bool isLinkExp = true;
    if( linkExprs == NULL || file == NULL || numExps == NULL ) isLinkExp = NULL;

    return NULL;
}

//-----------------------------------------------------------------------------


// Simplify
//-----------------------------------------------------------------------------

Node* GetSimplifiedConstantNode( Node* node, const char* varName, double val )
{
    ASSERT( node != NULL, 0 );

    if( node->value == NULL || !IS_OP ) return NULL; 

    double l_val = 0, r_val = 0;

    if/* */( IS_L_VAL )                                         l_val = L_VAL;
    else if( IS_L_VAR && varName && !strcmp( L_VAR, varName ) ) l_val = val;
    else if( !( !IS_L_EXISTS && IS_R_EXISTS ) )                 return NULL;

    if/* */( IS_R_VAL )                                         r_val = R_VAL;
    else if( IS_R_VAR && varName && !strcmp( R_VAR, varName ) ) r_val = val;
    else return NULL;

    // Simplify 
    int opNum = node->value->opValue;

    switch( opNum )
    {
        case OP_ADD:
            return CREATE_VAL_NODE( l_val + r_val );

        case OP_SUB:
            return CREATE_VAL_NODE( l_val - r_val );

        case OP_MUL:
            return CREATE_VAL_NODE( l_val * r_val );

        case OP_DIV:
            return CREATE_VAL_NODE( l_val / r_val );

        case OP_DEG:
            return CREATE_VAL_NODE( pow( l_val, r_val ) );   

        case OP_SIN:
            return CREATE_VAL_NODE( sin( r_val ) );

        case OP_COS:
            return CREATE_VAL_NODE( cos( r_val ) );
        
        case OP_LN:
            return CREATE_VAL_NODE( log( r_val ) );
    }    

    return NULL;
}

int SimplifyConstantsRecursively( Node* node, int* isWasSimpled, const char* varName, double val )
{
    ASSERT( node != NULL, 0 );
    
    if( node->left )
    {
        SimplifyConstantsRecursively( node->left, isWasSimpled, varName, val );
    }

    if( node->right )
    {
        SimplifyConstantsRecursively( node->right, isWasSimpled, varName, val );
    }

    Node* newNode = GetSimplifiedConstantNode( node, varName, val );

    if( newNode )
    {                
        ReplaceNode( node, newNode );

        (*isWasSimpled) = true;
    }

    return 1;
}

int SimplifyConstants( Node* node, const char* varName, double val )
{
    ASSERT( node != NULL, -1 );

    int isWasSimpled = false;

    SimplifyConstantsRecursively( node, &isWasSimpled, varName, val );

    return isWasSimpled;
}

Node* GetSimplifiedNeutralNode( Node* node )
{
    ASSERT( node != NULL, NULL );

    if( node->value == NULL ) return NULL; 

    if( IS_OP )
    {
        int op = node->value->opValue;
        
        switch( op )
        {
            case OP_ADD:
                // x + 0
                if( IS_R_VAL && CompareDoubles( R_VAL, 0 ) )
                {
                    return node->left;
                }
                // 0 + x
                if( IS_L_VAL && CompareDoubles( L_VAL, 0 ) )
                {
                    return node->right;
                }
                break;

            case OP_SUB:
                // x - 0
                if( IS_R_VAL && CompareDoubles( R_VAL, 0 ) )
                {
                    return node->left;
                }
                break;

            case OP_MUL:
                // x * 0 || 0 * x
                if( ( IS_L_VAL && CompareDoubles( L_VAL, 0 ) ) || 
                    ( IS_R_VAL && CompareDoubles( R_VAL, 0 ) ) )
                {    
                    return CREATE_VAL_NODE( 0 );
                }
                // x * 1 
                if( IS_R_VAL && CompareDoubles( R_VAL, 1 ) )
                {
                    return node->left;
                }
                // 1 * x
                if( IS_L_VAL && CompareDoubles( L_VAL, 1 ) )
                {
                    return node->right;
                }
                break;
        
            case OP_DIV:
                // x / 1
                if( IS_R_VAL && CompareDoubles( R_VAL, 1 ) )
                {
                    return node->left;
                } 
                break;

            default:
                return NULL;
        }
    }

    return NULL;
}

int SimplifyNeutralsRecursively( Node* node, int *isWasSimpled )
{
    ASSERT( node != NULL, 0 );
    
    if( node->left )
    {
        SimplifyNeutralsRecursively( node->left,  isWasSimpled );
    }

    if( node->right )
    {
        SimplifyNeutralsRecursively( node->right, isWasSimpled );
    }

    Node* newNode = GetSimplifiedNeutralNode( node );

    if( newNode )
    {                
        ReplaceNode( node, newNode );

        (*isWasSimpled) = true;
    }

    return 1;
}

int SimplifyNeutrals( Node* node )
{
    ASSERT( node != NULL, -1 );

    int isWasSimpled = false;

    SimplifyNeutralsRecursively( node, &isWasSimpled );

    return isWasSimpled;
}

int Simplify( Node* node )
{
    ASSERT( node != NULL, 0 );

        SimplifyConstants( node );
    if( SimplifyNeutrals ( node ) ) 
    {
        Simplify( node ); 
    }

    return 1;
}

//-----------------------------------------------------------------------------


// Calculate value at point
//-----------------------------------------------------------------------------

Node* CalcValueAtPoint( Node* node, const char* varName, double val, double* answer )
{
    ASSERT( node    != NULL, NULL );
    ASSERT( varName != NULL, NULL );

    Node* newNode = CopyNode( node );

    SimplifyConstants( newNode, varName, val );

    if( !newNode->left && !newNode->right && answer ) 
    {
        (*answer) = newNode->value->dblValue;
    }
    else if( answer )
    {
        (*answer) = POISON_DBL;
    }

    return newNode;
}  

//-----------------------------------------------------------------------------


// Create Latex
//-----------------------------------------------------------------------------

int IncludeImgToTex( const char* imgName, FILE* fileName, double scale )
{
    ASSERT( imgName  != NULL, 0 );
    ASSERT( fileName != NULL, 0 );

    fprintf( fileName, "\\begin{figure}[H]\n"
                       "\t\\centering\n"
                       "\t\\includegraphics[width=%lf\\linewidth]{\"%s\"}\n"
                       "\\end{figure}\n",
                        scale,
                        imgName );

    return 1;
}

int PrintBeginTex( FILE* texFile )
{  
    ASSERT( texFile != NULL, 0 );

    { // fprintf BeginTex
    fprintf( texFile,  
             "\\documentclass[a4paper, 12pt]{article}\n"
             "\n"
             "\\usepackage[T2A]{fontenc}\n"
             "\\usepackage[cp1251]{inputenc}\n"
             "\\usepackage[english,russian]{babel}\n"
             "\n"
             "\\usepackage{graphicx, float}\n"
             "\\usepackage{wrapfig}\n"
             "\n"
             "\\date{\\today}\n"
             "\\author{ Тот самый Супер Перец с Б01-208 }\n"
             "\\title{ \\textbf{ Методичка } } \n\n" );  
    }

    return 1;
}

int CreateDiffTexFile( const char* texFileName, Node* node, int nDiff, const char* varName, int nTaylor )
{
    ASSERT( texFileName != NULL, 0 );
    
    FILE* texFile = fopen( texFileName, "w" );

    PrintBeginTex( texFile );  

#define TEX_FORMULA( NODE ) \
    PrintFormula( NODE, texFile, FormulaType::LATEX );

#define PUT( STR ) \
    fprintf( texFile, STR );

    PUT( "\n\\begin{document}\n"
            "\\maketitle\n\n" );
    { // Tex body
        PUT( "$$ f(x) = " )  TEX_FORMULA( node )  PUT( " $$" ) 

        DiffGraphDump( node, "Original" );
    
        const char* graphName = "graph.png";
        CreateFuncGraphImg( node, graphName, 0.001, 10 );

        IncludeImgToTex( graphName, texFile, 0.8 );

        PUT( "Давайте теперь возьмем n-ую производную заданной функции: \n" );

        Node* newNode = Differentiate( node, varName );

        PUT( "$$ " ) TEX_FORMULA( newNode ) PUT( " $$" )
    }
    PUT( "\n\n\\end{document}\n" );

#undef PUT

    fclose( texFile );
    return 1;
}

int CreatePdfFromTex( const char* texFileName )
{
    ASSERT( texFileName != NULL, 0 );

    char     cmd[ MaxStrLen ] = "";
    sprintf( cmd, "pdflatex %s", texFileName ); 

    system(  cmd  );

    char name[ MaxStrLen ] = "";
    sscanf( texFileName, "%[^.]", name );

    sprintf( cmd, "rm -f %s.aux %s.log", name, name );
    system(  cmd  ); // delete .log .aux

    return 1;
}

//-----------------------------------------------------------------------------


// Create Function Graph
//-----------------------------------------------------------------------------

int CreateFuncGraphImg( Node* node, const char* imgName, double xMin, double xMax, const char* varName )
{
    ASSERT( node != NULL, 0 );

    char     filePlotName[ MaxStrLen ] = "";
    sprintf( filePlotName, "%s.p", imgName );
    FILE* filePlot = fopen( filePlotName, "w" );

    { // Graph description 
    fprintf( filePlot, "set terminal png size 800, 600\n"
                       "set output \"%s\"\n"
                       "set xlabel \"X\"\n" 
                       "set ylabel \"Y\"\n"
                       "set grid\n"
                       "plot '-' lt 3 linecolor 1 notitle\n", 
                       imgName );
    } 

    for( double x = xMin; x <= xMax; x += 0.0001 )
    {
        double ans = 0;
        CalcValueAtPoint( node, "x", x, &ans );

        if( ans == POISON_DBL ) 
        {
            fprintf( stderr, "ERROR: Not all variables are defined...\n" );
            fclose(  filePlot  );
            return 0;
        }

        fprintf( filePlot, "%f\t %f\n", x, ans );  
    }

    fclose(  filePlot  );

    char     cmd[ MaxStrLen ] = "";
    sprintf( cmd, ".\\gnuplot\\bin\\gnuplot.exe %s", filePlotName );
    system(  cmd  );

    remove( filePlotName );
    return 1;
}

//-----------------------------------------------------------------------------


// Taylor
//-----------------------------------------------------------------------------

Node* ExpandIntoTaylorSeries( Node* node, int n, double x_0 )
{
    ASSERT( node != NULL, 0 );

    Node* currNode = node;

    for( int i = 0; i < n; i++ )
    {
        
    }

    return NULL;
}

uint64_t Factorial( uint64_t num )
{
    if( num <= 1 ) return 1;

    return num * Factorial( num - 1 );
}

//-----------------------------------------------------------------------------


// DiffDump
//-----------------------------------------------------------------------------

int GraphVizNodes( Node* node, FILE* dotFile, int* nodeNum )
{
    ASSERT( dotFile != NULL && nodeNum != NULL, 0 );
    
    int leftNum  = 0;
    int rightNum = 0;
    
    if/* */( node->left )
    {
        leftNum  = GraphVizNodes( node->left,  dotFile, nodeNum );
    }    
    if( node->right )
    {   
        rightNum = GraphVizNodes( node->right, dotFile, nodeNum );
    }

    int   typeNum = VAL_TYPE;
    char* typeStr = NULL;
    char* color   = NULL;

    if( node->value != NULL ) typeNum = node->value->type; 

    if/* */( typeNum == Types::OP_TYPE  ) { typeStr = "op"  ; color = "lightgrey"  ; }
    else if( typeNum == Types::VAL_TYPE ) { typeStr = "val" ; color = "lightgreen" ; }
    else if( typeNum == Types::VAR_TYPE ) { typeStr = "var" ; color = "lightblue"  ; }

    char valueStr[ MaxStrLen ] = "";
    PrintDiffNodeValue( valueStr, node );
    
    fprintf( dotFile, "\tnode%d[ shape = record, style = \"filled\", fillcolor = \"%s\", label = \"{ p: %p | n: %p | { %s | %s } }\" ];\n", 
                       *nodeNum, color, node->parent, node, typeStr, valueStr );                                      

    if( node->left )
    {
        fprintf( dotFile, "\t\"node%d\" -> \"node%d\"\n", *nodeNum, leftNum );
    }
    if( node->right )
    {
        fprintf( dotFile, "\t\"node%d\" -> \"node%d\"\n", *nodeNum, rightNum );
    }

    (*nodeNum)++;
    return *nodeNum - 1;
}

//-----------------------------------------------------------------------------

FILE* DiffCreateDotDumpFile( Node* node, const char* fileName )
{
    ASSERT( node != NULL, NULL );

    FILE* tempDotFile = fopen( fileName, "w" );

    fprintf( tempDotFile, "digraph ListDump\n" );
    fprintf( tempDotFile, "{\n" );
    {
        int curNodeNum = 1;  
        GraphVizNodes( node, tempDotFile, &curNodeNum );
    }
    fprintf( tempDotFile, "}\n" );

    return tempDotFile;
}

//-----------------------------------------------------------------------------

int DiffGraphDump( Node* node, const char* str, ... )
{
    ASSERT( node != NULL, 0 );

    const char* tempDotFileName = "tempGraphVizTree.dot"; 
    FILE*       tempDotFile = DiffCreateDotDumpFile( node, tempDotFileName );
    fclose(     tempDotFile     );

    static int dumpNum = 0;

    char     graphName[MaxStrLen] = "";
    sprintf( graphName, "img/graph%d.png", dumpNum++ );

    CreateGraphVizImg( tempDotFileName, graphName, "png" );

    // Delete temp file
    remove( tempDotFileName );

    // Create html
    fclose( FileDiffDump );
    FileDiffDump = fopen( FileDiffDumpName, "a+" );

    va_list   arg = {0};
    va_start( arg, str );

    // Create html file
    fprintf( FileDiffDump, "<pre>" );
    fprintf( FileDiffDump, "<font size = 4>" );
   vfprintf( FileDiffDump, str, arg );
    fprintf( FileDiffDump, "</font>" );

    if( str ) 
    {
        fprintf( FileDiffDump, "\n\n" );
    }

    fprintf( FileDiffDump, "<img src = \"%s\", style = \" max-width: 95vw \">", graphName );
    fprintf( FileDiffDump, "<hr>" );

    va_end( arg );
    fclose( FileDiffDump );

    return 1;
}

//-----------------------------------------------------------------------------
// End DiffDump


// Get random value in [minVal; maxVal] 
//-----------------------------------------------------------------------------

int Rand( int minVal, int maxVal )
{
    srand( time( NULL ) );

    return rand() % ( maxVal - minVal + 1 ) + minVal;  
}

//-----------------------------------------------------------------------------