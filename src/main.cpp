
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

    const char* fileTexName = "Manual.tex";

    CreateDiffTexFile( fileTexName, &diffTree );
    CreatePdfFromTex ( fileTexName );

    DiffGraphDump( newNode, "Differentiate" ); 

    printf( "\n" );

    free( diffData );
}

//-----------------------------------------------------------------------------