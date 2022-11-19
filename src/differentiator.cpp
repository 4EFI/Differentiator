

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "differentiator.h"
#include "tree.h"
#include "LOG.h"
#include "my_assert.h"
#include "stack.h"
#include "file_algs.h"
#include "str_algs.h"

//-----------------------------------------------------------------------------

int LoadDiffDataRecursively( Node* node, const char* diffData, int side = 0 )
{
    ASSERT( node     != NULL, 0 );
    ASSERT( diffData != NULL, 0 );
	
    /*
    char* data = ( char* )calloc( MaxStr, sizeof( char ) );
    
    char curSym = fgetc( file );

    if( curSym == '(' )
    {
        fscanf( file, " \"%[^\"]\" ", data );

        TreeSetNodeValue( node, data );
    }


    // Adding left child
    while( curSym = fgetc( file ) ) 
    {
        if( curSym != ' ' ) break; // skip spaces
    }    

    if( curSym == ')' )
    {
        ungetc( curSym, file );
        LoadAkinatorData( TreeAddChild( node, "", LEFT_SIDE ), diffData, LEFT_SIDE ); 
    }

    if( curSym == ')' && side == LEFT_SIDE ) 
    {
        return 1;
    }

    // Adding right child
    while( curSym = fgetc( file ) ) 
    {
        if( curSym != ' ' ) break; // skip spaces
    }

    if( curSym == ')' )
    {   
        ungetc( curSym, file );
        LoadAkinatorData( TreeAddChild( node, "", RIGHT_SIDE ), diffData, RIGHT_SIDE );  
    }

    if( curSym == '}' && side == RIGHT_SIDE ) 
    {
        return 1;
    }
    */

    return 1;
}

int LoadDiffData( Tree* tree, const char* diffData ) 
{
    ASSERT( tree     != NULL, 0 );
    ASSERT( diffData != NULL, 0 );

    return LoadDiffDataRecursively( &tree->headNode, diffData );
}

//-----------------------------------------------------------------------------
