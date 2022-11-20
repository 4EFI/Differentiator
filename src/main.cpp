
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
    
    LoadDiffData( &diffTree, diffData );
    fclose( file );

    PrintInorderNodes( &diffTree.headNode, stdout );

    TreeGraphDump( &diffTree );

    free    (  diffData );
    TreeDtor( &diffTree );
}

//-----------------------------------------------------------------------------