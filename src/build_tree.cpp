
#include "my_assert.h"
#include "tree.h"
#include "differentiator.h"

//-----------------------------------------------------------------------------

/*

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

//-----------------------------------------------------------------------------

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
                currOp   = -1;
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

*/

//-----------------------------------------------------------------------------