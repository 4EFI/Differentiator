
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "differentiator.h"
#include "tree.h"
#include "my_assert.h"
#include "file_algs.h"
#include "str_algs.h"

#include "LOG.h"

//-----------------------------------------------------------------------------

int LoadLabData( VarValue** varValues, VarValue** errors, const char* str );

//-----------------------------------------------------------------------------

int main()
{
    const char* fileDataName = "lab_data.txt";
    FILE* file = fopen( fileDataName, "r" ); 

    char* labData = NULL;  
    ReadAllFile( file, &labData );

    // Divide buffer to strings
    String* diffDataStrs = NULL;
    int numStrs = DivideStr( labData, &diffDataStrs );

    // LoadTree
    Node* treeEq = LoadDiffDataTree( diffDataStrs[0].str );

    DiffGraphDump( treeEq );

    for( int i = 1; i < numStrs; i++ )
    {
        VarValue* varValues = NULL;
        VarValue* errors    = NULL; 

        int num = LoadLabData( &varValues, &errors, diffDataStrs[i].str );
        
        //if( num ) break;

        break;
    }

    printf( "\n" );

    fclose( file );
    free( labData );
}

//----------------------------------------------------------------------------- 

int LoadLabData( VarValue** varValues, VarValue** errors, const char* str )
{
    ASSERT( varValues != NULL, -1 )
    ASSERT( errors    != NULL, -1 )

    LOG( "%s", str );

    return 1;
}

//----------------------------------------------------------------------------- 