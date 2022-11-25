
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "my_assert.h"
#include "stack.h"
#include "LOG.h"

#define FREE( ptr ) \
    free( ptr );    \
    ptr = NULL;

//  Node functions
//-----------------------------------------------------------------------------

int NodeCtor( Node* node )
{
    ASSERT( node != NULL, 0 );

    node->value = ( DiffNode* )calloc( 1, sizeof( DiffNode ) );

    node->parent = NULL;

    node->left   = NULL;
    node->right  = NULL;

    return 1;
}  

//-----------------------------------------------------------------------------

int NodeDtor( Node* node )
{
    ASSERT( node != NULL, 0 );
    
    node->value = NODE_POISON;
    
    FREE( node->left  );
    FREE( node->right );

    return 1;
}

//-----------------------------------------------------------------------------


//  Tree functions
//-----------------------------------------------------------------------------

Node* TreeSetNodeValue( Node* node, TreeElem_t val )
{
    ASSERT( node != NULL, NULL );

    node->value = val;

    return node;
}

//-----------------------------------------------------------------------------

Node* TreeAddChild( Node* node, TreeElem_t val, int side )
{
    ASSERT( node != NULL, NULL );

    Node*     newNode = ( Node* )calloc( 1, sizeof( Node ) );
    NodeCtor( newNode );

    if( !val ) 
    {
        newNode->value = ( DiffNode* )calloc( 1, sizeof( DiffNode ) );
        newNode->value->type = -1;
    }
    else
    {
        newNode->value = val;
    }

    newNode->parent = node;
    newNode->side   = side;

    if/* */( side == NodeSides::LEFT_SIDE )
    {
        node->left = newNode;
    }
    else if( side == NodeSides::RIGHT_SIDE )
    {
        node->right = newNode;
    }

    return newNode;
}

//-----------------------------------------------------------------------------

Node* TreeSearch( Node* nodeBegin, TreeElem_t val )
{
    ASSERT( nodeBegin != NULL, 0 );

    Node* leftNode  = NULL;
    Node* rightNode = NULL;

    if( nodeBegin->left )
    {
        leftNode  = TreeSearch( nodeBegin->left,  val );
    }
    if( nodeBegin->right )
    {
        rightNode = TreeSearch( nodeBegin->right, val );
    }

    if( nodeBegin->value, val ) 
    {
        return nodeBegin;
    }
    else if( leftNode  ) return leftNode;
    else if( rightNode ) return rightNode;

    return NULL;
}

//-----------------------------------------------------------------------------


//  Other functions
//-----------------------------------------------------------------------------

int CreateGraphVizImg( const char* dotFileName, const char* fileName, const char* fileType )
{
    if(dotFileName == NULL || fileName == NULL || fileType == NULL) return 0; 

    char cmd[MaxStrLen] = "";

    sprintf( cmd, "dot %s -T %s -o %s", dotFileName, fileType, fileName );
    system(  cmd  );

    return 1;
}

//-----------------------------------------------------------------------------
