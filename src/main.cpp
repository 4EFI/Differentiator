
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
    Tree       diffTree = { 0 };
    TreeCtor( &diffTree );

    const char* fileDataName = "diff_data.txt";
    FILE* file = fopen( fileDataName, "r" ); 

    char* diffData = NULL;
    ReadAllFile( file, &diffData );

    LOG( "%s", diffData );
    
    LoadDiffData( &diffTree, diffData );
    fclose( file );

    free( diffData );
    TreeGraphDump( &diffTree );
}

//-----------------------------------------------------------------------------