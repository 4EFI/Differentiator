

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "differentiator.h"
#include "tree.h"
#include "LOG.h"
#include "my_assert.h"
#include "stack.h"
#include "file_algs.h"
#include "str_algs.h"
#include "LOG.h"

//-----------------------------------------------------------------------------

int SetDiffNode( Node* node, const char* diffData )
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

        sscanf( diffData, " %[^()]%n ", tempStr, &numReadSyms );

        int opType = GetOperationType( tempStr );

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

int LoadDiffData( Tree* tree, const char* diffData ) 
{
    ASSERT( tree     != NULL, 0 );
    ASSERT( diffData != NULL, 0 );

    Node* currNode = &tree->headNode;

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

        i += SetDiffNode( currNode, diffData + i );
        i --;
    }

    return 1;
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

int PrintOperation( FILE* file, int numOp, Operation* operations, int numOperations )
{
    ASSERT( operations != NULL, 0 );
    ASSERT( file       != NULL, 0 );

    for( int i = 0; i < numOperations; i++ )
    {
        if( operations[i].opType == numOp )
        {
            fprintf( file, "%s", operations[i].opStr );
            return 1;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------

int PrintInorderNodes( Node* node, FILE* file ) 
{
    ASSERT( node != NULL, 0 );
    ASSERT( file != NULL, 0 );  

    fprintf( file, "( " ); 

    if( node->value->type == OP_TYPE && node->value->opValue == OP_DIV )
    {
        fprintf( file, "dfrac{ " );
    }

    if( node->left )  
    {
        PrintInorderNodes( node->left, file );
    }
    
    if/* */( node->value->type == VAL_TYPE )
    {
        fprintf( file, "%g", node->value->dblValue );
    }
    else if( node->value->type == OP_TYPE )
    {
        PrintOperation( stdout, node->value->opValue );
    }
    else if( node->value->type == VAR_TYPE )
    {
        fprintf( file, "%s", node->value->varValue );
    }
    
    if( node->right ) 
    {
        PrintInorderNodes( node->right, file );
    }
    
    fprintf( file, " )" );

    return 1;
}

//-----------------------------------------------------------------------------