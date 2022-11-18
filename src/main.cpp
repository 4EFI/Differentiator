
#include "config.h"

#include "differentiator.h"
#include "tree.h"

//-----------------------------------------------------------------------------

int main()
{
    Tree       diffTree = { 0 };
    TreeCtor( &diffTree );

    const char* fileDataName = "diff_data.txt";
    FILE* file = fopen( fileDataName, "r" ); 
    
    LoadDiffData( &diffTree.headNode, file );
    fclose( file );

    TreeDump( &diffTree );
}

//-----------------------------------------------------------------------------