
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

    PrintFormula ( &diffTree, stdout, FormulaType::LATEX );
    DiffGraphDump( &diffTree, "Original" );
    
    printf( "\n" );

    Node* newNode = Differentiate( &diffTree, "a" );

    DiffGraphDump( newNode, "Differentiate" ); 

    Simplify     ( newNode );
    PrintFormula ( newNode,  stdout, FormulaType::LATEX );
    DiffGraphDump( newNode, "Simplify" ); 

    printf( "\n" );

    double ans = CalcValueAtPoint( newNode, "a", 2 ); 

    LOG( "%lf", ans );

    CreateTexFile( "Manual.tex", newNode );

    //CreatePdfFromTex( "Manual.tex" );

    free( diffData );
}

//-----------------------------------------------------------------------------