
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

        double ans   = CalcValueAtPoint( treeEq, varValues, num );
        double error = CalcErrorAtPoint( treeEq, varValues, errors, num );

        LOG( "ans = %lf +- %lf", ans, error );

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

    char* str_ptr = ( char* )str;

    int num = 0;
    while( true )
    {
        char   tempStr[ MaxStrLen ] = "";
        double dbl;

        int numReadSyms = 0;
        int numRead     = sscanf( str_ptr, "%s %lf%n", tempStr, &dbl, &numReadSyms );

        str_ptr += numReadSyms;

        if( numRead <= 0 ) break;

        num++;
    }

    num /= 2;

    LOG( "Num = %d", num );
    
    *varValues = ( VarValue* )calloc( num, sizeof( VarValue ) );
    *errors    = ( VarValue* )calloc( num, sizeof( VarValue ) );

    str_ptr = ( char* )str;

    for( int i = 0; i < num; i++ )
    {
        char* varValueStr = ( char* )calloc( MaxStrLen, 1 );
        char* errorStr    = ( char* )calloc( MaxStrLen, 1 );
        
        int numReadSyms = 0;
        sscanf( str_ptr, "%s %lf %s %lf%n", varValueStr, &(*varValues)[i].value,
                                            errorStr,    &(*errors)   [i].value,
                                           &numReadSyms                          );

        (*varValues)[i].var = varValueStr;
        (*errors)   [i].var = errorStr;

        str_ptr += numReadSyms;

        LOG( "%s %lf %s %lf", (*varValues)[i].var, (*varValues)[i].value, 
                              (*errors)   [i].var, (*errors)   [i].value );
    }

    return num;
}

//----------------------------------------------------------------------------- 