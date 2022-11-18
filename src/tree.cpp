
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

FILE* FileTreeDump = fopen( FileTreeDumpName, "w" );

//  Node functions
//-----------------------------------------------------------------------------

int NodeCtor( Node* node )
{
    ASSERT( node != NULL, 0 );

    node->parent = NULL;

    node->value  = NULL;
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

int PrintPreorderNodes( Node* node, FILE* file ) 
{
    ASSERT( node != NULL && file != NULL, 0 );

    fprintf( file, "{ \"%s\" ", node->value );

    if( node->left )  
    {
        PrintPreorderNodes( node->left, file );
    }
    if( node->right ) 
    {
        PrintPreorderNodes( node->right, file );
    }
    
    fprintf( file, "} " );

    return 1;
}

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
    
    fprintf( dotFile, "\tnode%d[ shape = record, style = \"filled\", fillcolor = \"%s\", label = \"%s\" ];\n", 
                       *nodeNum, 
                        node->left == NULL &&  node->right == NULL ? "lightgrey" : "lightgreen",
                        node->value );                                      

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


//  Tree functions
//-----------------------------------------------------------------------------

int TreeCtor( Tree* tree )
{
    ASSERT( tree != NULL, 0 );

    tree->isEmpty = true;

    NodeCtor( &tree->headNode );

    return 1;
}

//-----------------------------------------------------------------------------

int TreeDtor( Tree* tree )
{
    ASSERT( tree != NULL, 0 );
    
    // soon

    return 1;
}

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

    newNode->value  = val;
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

FILE* TreeCreateDotDumpFile( Node* node, const char* fileName )
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

int TreeGraphDump( Tree* tree )
{
    ASSERT( tree != NULL, 0 );

    fclose( FileTreeDump );
    FileTreeDump = fopen( FileTreeDumpName, "w" );

    const char* tempDotFileName = "tempGraphVizTree.dot"; 
    FILE*       tempDotFile = TreeCreateDotDumpFile( &tree->headNode, tempDotFileName );
    fclose(     tempDotFile     );

    char graphName[MaxStrLen] = "img/graph.png";

    CreateGraphVizImg( tempDotFileName, graphName, "png" );

    // Delete temp file
    remove( tempDotFileName );

    // Create html file
    fprintf( FileTreeDump, "<pre>" );

    fprintf( FileTreeDump, "<img src = \"%s\", style = \" max-width: 95vw \">", graphName );
    fprintf( FileTreeDump, "<hr>" );

    fclose( FileTreeDump );

    return 1;
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

    if/* */( stricmp( nodeBegin->value, val ) == 0 ) 
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
