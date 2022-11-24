
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

    node->value = ( DiffNode* )calloc( 1, sizeof( DiffNode ) );
    
    int numReadSyms = 0;

    double num = 0;
    bool isNum = sscanf( diffData, " %lf%n ", &num, &numReadSyms );

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

int LoadDiffData( Node* node, const char* diffData ) 
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
            if( !currNode->left )
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

        if( IsUnaryOperation( currOp ) >= 0 )
        {
            TreeAddChild( currNode, NULL, LEFT_SIDE );
        }
    }

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

int PrintLatexFormulaRecursively( Node* node, FILE* file ) 
{
    ASSERT( node != NULL, 0 );
    ASSERT( file != NULL, 0 );

    if( node->value == NULL ) return 0; 

    if( node->left && node->right && node->parent != NULL )
    {
        fprintf( file, "( " );  
    }
    PrintTex( Begin );

    if( node->left )  
    {
        PrintLatexFormulaRecursively( node->left, file );
    }
    
    if( node->value->type != OP_TYPE )
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
        PrintLatexFormulaRecursively( node->right, file );
    }

    PrintTex( End );
    if( node->left && node->right && node->parent != NULL )
    {
        fprintf( file, " )" );  
    }

    return 1;
}

int PrintLatexFormula( Node* node, FILE* file )
{
    ASSERT( node != NULL, 0 );
    ASSERT( file != NULL, 0 );

    fprintf( file, "$$ " );
    
    PrintLatexFormulaRecursively( node, file );

    fprintf( file, " $$" );

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

Node* CreateNode( int type, double dbl, int op, char* var, Node* left, Node* right, Node* node )
{    
    Node* newNode = ( Node* )calloc( 1, sizeof( Node ) );

    newNode->value           = ( DiffNode* )calloc( 1, sizeof( DiffNode ) );    
    newNode->value->type     = type;
    newNode->value->dblValue = dbl;
    newNode->value->opValue  = op;
    newNode->value->varValue = var;

    newNode->parent = node;
    newNode->left   = left;
    newNode->right  = right;

    return newNode;
}

Node* CopyNode( Node* node )
{
    ASSERT( node != NULL, 0 );

    Node* newNode = ( Node* )calloc( 1, sizeof( Node ) );

    newNode->value = ( DiffNode* )calloc( 1, sizeof( DiffNode ) );

    memcpy( newNode,        node,        sizeof(     Node ) );
    memcpy( newNode->value, node->value, sizeof( DiffNode ) );

    if( node->left )
    {
        newNode->left  = CopyNode( node->left );
    }

    if( node->right )
    {
        newNode->right = CopyNode( node->right );
    }

    return newNode;
}

//-----------------------------------------------------------------------------


// Differentiate
//-----------------------------------------------------------------------------

Node* DifferentiateNode( Node* node )
{
    ASSERT( node        != NULL, 0 );
    ASSERT( node->value != NULL, 0 );

    switch( node->value->type )
    {
        case VAL_TYPE:
            return CREATE_VAL_NODE( 0 );

        case VAR_TYPE:
            if( *(node->value->varValue) == 'x' ) 
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
                    return DIV(  SUB( MUL( DL, CR ), MUL( CL, DR ) ), MUL( CR, CR )  );

                case OP_SIN:
                    return MUL(  COS( NULL, CR ), DR  );

                case OP_COS:
                    return MUL(MUL(SIN(NULL, CR), CREATENUM(-1)), DR);
                case OP_POWER:
                    return DiffPower(node);
                case OP_LOG:
                    return DiffLog(node);
                case OP_LN:
                    return MUL(DIV(CREATENUM(1), CR), DR);
                case OP_TG:
                    return MUL(DIV(CREATENUM(1), POWER(COS(NULL, CR), CREATENUM(2))), DR);
                case OP_CTG:
                    return MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), POWER(SIN(NULL, CR), CREATENUM(2)))), DR);
                case OP_SQRT:
                    return MUL(DIV(CREATENUM(1), MUL(CREATENUM(2), SQRT(NULL, CR))), DR);
                case OP_ARCSIN:
                    return MUL(DIV(CREATENUM(1), SQRT(NULL, SUB(CREATENUM(1), POWER(CR, CREATENUM(2))))), DR);
                case OP_ARCCOS:
                    return MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), SQRT(NULL, SUB(CREATENUM(1), POWER(CR, CREATENUM(2)))))), DR);
                case OP_ARCTG:
                    return MUL(DIV(CREATENUM(1), ADD(CREATENUM(1), POWER(CR, CREATENUM(2)))), DR);
                case OP_ARCCTG:
                    return MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), ADD(CREATENUM(1), POWER(CR, CREATENUM(2))))), DR);
                case OP_SH:
                    return MUL(CH(NULL, CR), DR);
                case OP_CH:
                    return MUL(SH(NULL, CR), DR);
                case OP_UNKNOWN:
                    return node;
                default:
                    return node;
            */
            }
        default:
            break;
    }

    return node;
}

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

Node* Differentiate( Node* node )
{
    ASSERT( node != NULL, 0 );

    Node* newNode = DifferentiateNode( node );

    LinkNodeParents( newNode, NULL );

    return newNode;
}

//-----------------------------------------------------------------------------


// Simplify
//-----------------------------------------------------------------------------

Node* GetSimplifiedConstantNode( Node* node )
{
    ASSERT( node != NULL, 0 );

    if( node->value == NULL ) return NULL; 

    if( IS_R_VAL )
    {
        if( IS_L_VAL )
        {
            int opNum = node->value->opValue;

            switch( opNum )
            {
                case OP_ADD:
                    return CREATE_VAL_NODE( L_VAL + R_VAL );

                case OP_SUB:
                    return CREATE_VAL_NODE( L_VAL - R_VAL );

                case OP_MUL:
                    return CREATE_VAL_NODE( L_VAL * R_VAL );

                case OP_DIV:
                    return CREATE_VAL_NODE( L_VAL / R_VAL );

                case OP_DEG:
                    return CREATE_VAL_NODE( pow( L_VAL, R_VAL ) );
            
                default:
                    return NULL;
            }            
        }
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
        if/* */( IS_L ) node->parent->left  = newNode;
        else if( IS_R ) node->parent->right = newNode;

        if( node->parent == 0 ) memcpy( node, newNode, sizeof( Node ) );

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
                if( IS_L_VAR && IS_R_VAL && CompareDoubles( R_VAL, 0 ) )
                {
                    return CREATE_VAR_NODE( L_VAR );
                }
                // 0 + x
                if( IS_R_VAR && IS_L_VAL && CompareDoubles( L_VAL, 0 ) )
                {
                    return CREATE_VAR_NODE( R_VAR );
                }
                break;

            case OP_SUB:
                // x - 0
                if( IS_L_VAR && IS_R_VAL && CompareDoubles( R_VAL, 0 ) )
                {
                    return CREATE_VAR_NODE( L_VAR );
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
                break;
        }
    }

    return NULL;
}

int SimplifyNeutralsRecursively( Node* node, int *isWasSimpled )
{
    ASSERT( node != NULL, 0 );
    
    if( node->left )
    {
        SimplifyNeutralsRecursively( node->left, isWasSimpled );
    }

    if( node->right )
    {
        SimplifyNeutralsRecursively( node->right, isWasSimpled );
    }

    Node* newNode = GetSimplifiedNeutralNode( node );

    if( newNode )
    {        
        if/* */( IS_L ) node->parent->left  = newNode;
        else if( IS_R ) node->parent->right = newNode;

        if( node->parent == 0 ) memcpy( node, newNode, sizeof( Node ) );

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

    int isWasSimpled = 0;

    isWasSimpled += SimplifyConstants( node );
    isWasSimpled += SimplifyNeutrals ( node );

    if( isWasSimpled ) Simplify( node ); 

    return 1;
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
    
    fprintf( dotFile, "\tnode%d[ shape = record, style = \"filled\", fillcolor = \"%s\", label = \"%s | %s \" ];\n", 
                       *nodeNum, color, typeStr, valueStr );                                      

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
