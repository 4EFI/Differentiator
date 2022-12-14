
#include "config.h"

#include <stdlib.h>
#include <time.h>

#include "differentiator.h"
#include "tree.h"
#include "file_algs.h"
#include "str_algs.h"

#include "LOG.h"

//-----------------------------------------------------------------------------

int main()
{
    srand( time( NULL ) );
    
    const char* fileDataName = "diff_data.txt";
    FILE* file = fopen( fileDataName, "r" ); 

    char* diffData = NULL;
    ReadAllFile( file, &diffData );

    // Divide buffer to strings
    String* diffDataStrs = NULL;
    int numStrs = DivideStr( diffData, &diffDataStrs );

    // LoadTree
    Node* diffTree = LoadDiffDataTree( diffDataStrs[0].str );

    // Load Nums
    int nDiff = 0, nTaylor = 0;
    char varName[ MaxStrLen ] = "";

    double xTaylor = 0;
    
    int numValues = 0;
    VarValue* arrVarValue = NULL;
    VarValue* errors      = NULL; 

    LoadDiffDataNums( diffDataStrs, numStrs, &nDiff, varName, &nTaylor );

    LOG( "nDiff = %d varName = \"%s\" nTaylor = %d", nDiff, varName, nTaylor );

    const char* fileTexName = "Manual.tex";

    CreateDiffTexFile( fileTexName, diffTree, nDiff, varName, nTaylor );
    CreatePdfFromTex ( fileTexName );

    printf( "\n" );

    fclose( file );
    free( diffData );
}

//----------------------------------------------------------------------------- 