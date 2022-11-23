
#include "config.h"

#include <stdlib.h>

#include "differentiator.h"
#include "tree.h"
#include "file_algs.h"
#include "str_algs.h"

#include "LOG.h"

//-----------------------------------------------------------------------------

int main()
{
    Node       diffTree = { 0 };
    NodeCtor( &diffTree );
    
    const char* fileDataName = "diff_data.txt";
    FILE* file = fopen( fileDataName, "r" ); 

    char* diffData = NULL;
    ReadAllFile( file, &diffData );
    
    LoadDiffData( &diffTree, diffData );
    fclose( file );

    PrintLatexFormula( &diffTree, stdout );
    DiffGraphDump    ( &diffTree, "Original" );
    
    printf( "\n" );

    Node* newNode = Differentiate( &diffTree );

    PrintLatexFormula( newNode,  stdout );
    DiffGraphDump    ( newNode, "Differentiate" );

    printf( "\n" );

    SimplifyConstant ( newNode );
    PrintLatexFormula( newNode,  stdout );
    DiffGraphDump    ( newNode, "Simplify" );


    free( diffData );
}

//-----------------------------------------------------------------------------