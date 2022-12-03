
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

int LoadDiffDataNums( String* diffDataStrs, int numStrs, 
                      int* nDiff, char* varName, 
                      int* nTaylor )
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

    bool isBracket = ( ( node->left - node->right )       &&

                      !( node->value->opValue >= OP_DIV   && 
                         node->value->opValue <= OP_COS ) &&

                         node->parent != NULL             &&

		              !( node->value->opValue - node->parent->value->opValue > 1 ) );

    if( isBracket ) fprintf( file, "( " );  
    
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

    if( isBracket ) fprintf( file, " )" );  

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

Node* CopyNode( const Node* node )
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

Node* DifferentiateNode( const Node* node, const char* varName )
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
            }
        default:
            break;
    }

    return ( Node* )node;
}

int IsVarInTree( const Node* node, const char* varName )
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

Node* Differentiate( const Node* node, const char* varName )
{
    ASSERT( node != NULL, 0 );

    Node* newNode = ( Node* )node;

    newNode = DifferentiateNode( newNode, varName );
    LinkNodeParents( newNode, NULL );

    return newNode;
}

Node* DifferentiateN( const Node* node, const char* varName, int n, FILE* file )
{
    ASSERT( node    != NULL, 0 );
    ASSERT( varName != NULL, 0 );
    ASSERT( file    != NULL, 0 );
 
    if( n <= 0 ) return ( Node* )node;


    Node* newNode = ( Node* )node;

#define PRINT_RAND_PHRASE \
    if( file != NULL ) fprintf( file, "\n%s:\n", LinkExprs[ Rand(0, NumLinkExprs - 1) ] );

#define PUT( STR, ... ) \
    if( file != NULL ) fprintf( file, STR, ##__VA_ARGS__ );

#define TEX_FORMULA( NODE ) \
    if( file != NULL ) PrintFormula( NODE, file, FormulaType::LATEX );

    for( int i = 1; i <= n; i++ )
    {
        newNode = Differentiate( newNode, varName );
        PRINT_RAND_PHRASE

        DiffGraphDump( newNode, "Differentiate" ); // Dump

        PUT( "\n\n$$ f^{(%d)}(%s) = ", i, varName ) TEX_FORMULA( newNode ) PUT( " $$\n\n" )

        Simplify( newNode );
        PRINT_RAND_PHRASE

        DiffGraphDump( newNode, "Simplify" ); // Dump

        PUT( "\n\n$$ f^{(%d)}(%s) = ", i, varName ) TEX_FORMULA( newNode ) PUT( " $$\n\n" )
    }

#undef PRINT_RAND_PHRASE
#undef PUT
#undef TEX_FORMULA

    return newNode;
}

//-----------------------------------------------------------------------------


// Simplify
//-----------------------------------------------------------------------------

Node* GetSimplifiedConstantNode( Node* node )
{
    ASSERT( node != NULL, NULL );

    if( node->value == NULL || !IS_OP ) return NULL;

    
    double l_val = 0, r_val = 0;

    if( IS_L_VAL ) 
    {
        l_val = L_VAL;
    }
    else if( !( !IS_L_EXISTS && IS_R_EXISTS ) ) return NULL;

    if( IS_R_VAL ) 
    {
        r_val = R_VAL;
    }
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

int SimplifyConstantsRecursively( Node* node, int* isWasSimpled )
{
    ASSERT( node != NULL, 0 );
    
    if( node->left )
    {
        SimplifyConstantsRecursively( node->left, isWasSimpled );
    }

    if( node->right )
    {
        SimplifyConstantsRecursively( node->right, isWasSimpled );
    }

    Node* newNode = GetSimplifiedConstantNode( node );

    if( newNode )
    {                
        ReplaceNode( node, newNode );

        (*isWasSimpled) = true;
    }

    return 1;
}

int SimplifyConstants( Node* node )
{
    ASSERT( node != NULL, -1 );

    int isWasSimpled = false;

    SimplifyConstantsRecursively( node, &isWasSimpled );

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

            case OP_DEG:
                // x^1 
                if( IS_R_VAL && CompareDoubles( R_VAL, 1 ) )
                {
                    return node->left;
                }
                // 1^x
                if( IS_L_VAL && CompareDoubles( L_VAL, 1 ) )
                {
                    return CREATE_VAL_NODE( 1 );
                }
                // x^0
                if( IS_R_VAL && CompareDoubles( R_VAL, 0 ) )
                {
                    return CREATE_VAL_NODE( 1 );
                }

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


// Calculate value/error at point
//-----------------------------------------------------------------------------

int FuncSubstituteVarValuesRecursively( const Node* node, const VarValue arrVarValue[], int num )
{
    ASSERT( node        != NULL, 0 );
    ASSERT( arrVarValue != NULL, 0 );

    if( IS_VAR )
    {
        for( int i = 0; i < num; i++ )
        {
            if( !strcmp( node->value->varValue, arrVarValue[i].var ) )
            {
                Node* valNode = CREATE_VAL_NODE( arrVarValue[i].value );
                
                valNode->left  = node->left;
                valNode->right = node->right; 
                
                ReplaceNode( ( Node* )node, valNode );
            }
        }
    }

    if( node->left )
    {
        FuncSubstituteVarValuesRecursively( node->left,  arrVarValue, num );
    }
    
    if( node->right )
    {
        FuncSubstituteVarValuesRecursively( node->right, arrVarValue, num );
    }

    return 1;
}

Node* FuncSubstituteVarValues( const Node* node, const VarValue arrVarValue[], int num )
{
    ASSERT( node        != NULL, NULL );
    ASSERT( arrVarValue != NULL, NULL );

    Node* newNode = CopyNode( node );

    FuncSubstituteVarValuesRecursively( newNode, arrVarValue, num );
    Simplify( newNode );

    return newNode;
}

Node* FuncSubstituteVarValues( const Node* node, const char* varName, double val )
{
    ASSERT( node    != NULL, NULL );
    ASSERT( varName != NULL, NULL );

    VarValue varValue = { varName, val }; 

    return FuncSubstituteVarValues( node, &varValue, 1 );
}

Node* CalcValueAtPoint( const Node* node, const VarValue arrVarValue[], int num, double* answer )
{
    ASSERT( node        != NULL, NULL );
    ASSERT( arrVarValue != NULL, NULL );

    Node*     newNode = FuncSubstituteVarValues( node, arrVarValue, num );
    Simplify( newNode );

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

Node* CalcValueAtPoint( const Node* node, const char* varName, double val, double* answer )
{
    ASSERT( node    != NULL, NULL );
    ASSERT( varName != NULL, NULL );

    VarValue varValue = { varName, val };

    return CalcValueAtPoint( node, &varValue, 1, answer );
}  

double CalcErrorAtPoint( const Node* node, const VarValue arrVarValue[], const VarValue errors[], int num, Node** errorNode )
{
    ASSERT( node        != NULL, NULL );
    ASSERT( arrVarValue != NULL, NULL );

    Node* newNode = CREATE_VAL_NODE( 0 );

    for( int i = 0; i < num; i++ )
    {
        Node* diffNode = Differentiate( node, arrVarValue[i].var );

        // (f'(x) * dx)^2
        Node* tempNode = POW(  MUL( diffNode, CREATE_VAR_NODE( errors[i].var ) ), CREATE_VAL_NODE( 2 )  );

        newNode = ADD( newNode, tempNode ); 
        
        LinkNodeParents( newNode, NULL );
    } 

    // sqrt( all )
    Node* errorNodeLocal = POW(  newNode, CREATE_VAL_NODE( 0.5 )  );

    LinkNodeParents( errorNodeLocal, NULL );
    Simplify       ( errorNodeLocal );

    if( errorNode ) *errorNode = errorNodeLocal;

    errorNodeLocal = FuncSubstituteVarValues( errorNodeLocal, arrVarValue, num );
    errorNodeLocal = FuncSubstituteVarValues( errorNodeLocal, errors,      num );
    
    if( errorNodeLocal->left || errorNodeLocal->right ) return POISON_DBL;
    
    return errorNodeLocal->value->dblValue;
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
             "\\documentclass{article}\n"
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

int CreateDiffTexFile( const char* texFileName, 
                       Node*       node, 
                       int         nDiff, 
                       const char* varName, 
                       int         nTaylor )
{
    ASSERT( texFileName != NULL, 0 );
    
    FILE* texFile = fopen( texFileName, "w" );

    PrintBeginTex( texFile );  

#define TEX_FORMULA( NODE ) \
    PrintFormula( NODE, texFile, FormulaType::LATEX );

#define PUT( STR, ... ) \
    fprintf( texFile, STR, ##__VA_ARGS__ );

    PUT( "\n\\begin{document}\n"
            "\\maketitle\n\n" );
    { // Tex body
        
        PUT( "\\subsection{ Производная }\n" )
        PUT( "Перец блин ашалел, когда такую функцию увидел:\n" )
        
        PUT( "\n\n$$ f(x) = " )  TEX_FORMULA( node )  PUT( " $$\n\n" ) 

        PUT( "Дай хоть посмотрю на тебя, может проще станет...\n" )

        DiffGraphDump( node, "Original" ); // Dump
    
        const char* graphName = "graph.png";
        CreateFuncGraphImg( node, graphName, 0.001, 10 );

        IncludeImgToTex( graphName, texFile, 0.8 );

        PUT( "Мдааааа, ну и говнище. Стоп. Какую-какую производную брать? %d? " 
             "Ты меня за кого принимаешь, за прогу, которая может взять любую производную? "
             "А блин, меня же учили на первом курсе, ладно, давай сюда свою функцию:\n\\\\\n", nDiff )

        Node* newNode = DifferentiateN( node, varName, nDiff, texFile );

        //

        PUT( "\\subsection{ Разложение в ряд Тейлора }\n" )
        PUT( "А я и не знал, что я так умею. Ну раз n-ую производную взял, то и в ряд Тейлора разложу. "
             "Ну что же, давай попробую, может что и выйдет:\n\\\\\n" )

        Node* taylorNode = ExpandIntoTaylorSeries( node, varName, nTaylor, 2 );

        PUT( "\n\n$$ f(x) = " )  TEX_FORMULA( taylorNode )  PUT( " $$\n\n" ) 

        DiffGraphDump( taylorNode, "Taylor" ); // Dump

        PUT( "Давай я чутка упрощу, а то громоздко выгядит:\n" )
        
        Simplify( taylorNode );

        PUT( "\n\n$$ f(x) = " )  TEX_FORMULA( taylorNode )  PUT( " $$\n\n" ) 

        PUT( "Ну вот, совсем другое дело, оказалось и не так сложно, а главное все очевидно.\n\n" )

        DiffGraphDump( taylorNode, "Simplify Taylor" ); // Dump

        //

        PUT( "\\subsection{ Уравнение касательной в точке }\n" )
        PUT( "Ну и запросы у тебя, уравнение касательной в точке захотел. Ладно. Хорошо. "
             "А ты знал, что это очень просто сделать: $t(x) = f^{(1)}(x)(x - x_0) + f(x_0)$. "
             "Руководствуясь этим, получаем уравнение "
             "касательной к графику в точке %s = %d:\n\\\\\n", varName, 0 )

        Node* tangentNode = GetTangentEquationAtPoint( node, varName, 2 );

        DiffGraphDump( tangentNode, "Tangent" ); // Dump

        PUT( "\n\n$$ t(%s) = ", varName )  TEX_FORMULA( tangentNode )  PUT( " $$\n\n" ) 

        //

        PUT( "\\subsection{ Погрешность }" )
        PUT( "Ну что там еще, хочешь лабу быстро делать, а знаешь сколько я училась такому. "
             "Ну ладно, напрягусь разок. Но это последний, понял? Вообще это предельно легко сделать: "
            "$\\sigma = \\sqrt{ (f'_a \\cdot da)^2 + \\dots + (f'_z \\cdot dz)^2 }$. "
            "Ну давай, используя это, помогу тебе:\n\\\\\n" )

        VarValue errors[]      = { {"dx", 2}, {"dn", 4} };
        VarValue arrVarValue[] = { {"x",  2}, {"n",  3} };

        Node* errorNode = NULL;

        double error = CalcErrorAtPoint( node, arrVarValue, errors, 2, &errorNode );

        DiffGraphDump( errorNode, "Error" );

        PUT( "\n\n$$ \\sigma = ", varName )  TEX_FORMULA( errorNode )  PUT( " = %lf $$\n\n", error ) 
    }
    PUT( "\n\n\\end{document}\n" );

#undef PUT
#undef TEX_FORMULA

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

Node* ExpandIntoTaylorSeries( Node* node, const char* varName, int n, double x_0 )
{
    ASSERT( node    != NULL, 0 );
    ASSERT( varName != NULL, 0 );

    Node* newNode  = FuncSubstituteVarValues( node, varName, x_0 );
    Node* diffNode = node;

    int currFact = 1;

// ( x - x_0 )
#define SUB_NODE \
    CREATE_OP_NODE(  OP_SUB, CREATE_VAR_NODE( varName ), CREATE_VAL_NODE( x_0 )  )

    for( int i = 1; i <= n; i++ )
    {   
        currFact *= i;
        
        diffNode = Differentiate( diffNode, varName );

        Node* calcedNode = FuncSubstituteVarValues( diffNode, varName, x_0 );

        newNode = ADD(  newNode, DIV( MUL( calcedNode, POW( SUB_NODE, CREATE_VAL_NODE( i ) ) ), CREATE_VAL_NODE( currFact ) )  );
    }

    // o( (x - x_0)^n
    Node* smallNode = CREATE_OP_NODE(  OP_O, NULL, POW( SUB_NODE, CREATE_VAL_NODE( n ) )  );

    // newNode + o( (x - x_0)^n )
    newNode = CREATE_OP_NODE(  OP_ADD, newNode, smallNode  ); 

#undef SUB_NODE

    LinkNodeParents( newNode, NULL );                                
    return newNode;
}

uint64_t Factorial( uint64_t num )
{
    if( num <= 1 ) return 1;

    return num * Factorial( num - 1 );
}

//-----------------------------------------------------------------------------


// Tangent equation
//-----------------------------------------------------------------------------

Node* GetTangentEquationAtPoint( Node* node, const char* varName, double val )
{
    ASSERT( node    != NULL, NULL );
    ASSERT( varName != NULL, NULL );

    Node*  diffNode   = Differentiate          ( node,     varName );
    Node*  fX_0       = FuncSubstituteVarValues( node,     varName, val );
    Node*  calcedNode = FuncSubstituteVarValues( diffNode, varName, val );

    Node*            newNode = ADD(  MUL( calcedNode, CREATE_VAR_NODE( varName ) ), SUB( fX_0, MUL( calcedNode, CREATE_VAL_NODE( val ) ) )  ); 
    LinkNodeParents( newNode, NULL );
    Simplify       ( newNode );

    return newNode;
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
    return rand() % ( maxVal - minVal + 1 ) + minVal;  
}

//-----------------------------------------------------------------------------