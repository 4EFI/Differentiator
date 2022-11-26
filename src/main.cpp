
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

    Node* newNode  = Differentiate( &diffTree, "x" );
    Node* newNode2 = Differentiate( newNode,   "x" );

    DiffGraphDump( newNode, "Differentiate" ); 

    Simplify     ( newNode2 );
    PrintFormula ( newNode2,  stdout, FormulaType::LATEX );
    DiffGraphDump( newNode2, "Simplify" ); 

    printf( "\n" );

    double ans = 0;
    PrintFormula( CalcValueAtPoint( newNode2, "x", 2, &ans ) );

    CreateTexFile   ( "Manual.tex", newNode2 );
    CreatePdfFromTex( "Manual.tex" );

    free( diffData );
}

//-----------------------------------------------------------------------------