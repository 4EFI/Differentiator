

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "differentiator.h"
#include "tree.h"
#include "LOG.h"
#include "my_assert.h"
#include "stack.h"

//-----------------------------------------------------------------------------

int LoadDiffDataRecursive( Node* node, FILE* file, int side )
{
    ASSERT( node != NULL, 0 );
    ASSERT( file != NULL, 0 );
	
	char* data = ( char* )calloc( MaxStr, sizeof( char ) );
    
    char curSym = fgetc( file );

    if( curSym == '{' )
    {
        fscanf( file, " \"%[^\"]\" ", data );

        TreeSetNodeValue( node, data );
    }


    // Adding left child
    while( curSym = fgetc( file ) ) 
    {
        if( curSym != ' ' ) break; // skip spaces
    }    

    if( curSym == '{' )
    {
        ungetc( curSym, file );
        LoadAkinatorData( TreeAddChild( node, "", LEFT_SIDE ), file, LEFT_SIDE ); 
    }

    if( curSym == '}' && side == LEFT_SIDE ) 
    {
        return 1;
    }

    // Adding right child
    while( curSym = fgetc( file ) ) 
    {
        if( curSym != ' ' ) break; // skip spaces
    }

    if( curSym == '{' )
    {   
        ungetc( curSym, file );
        LoadAkinatorData( TreeAddChild( node, "", RIGHT_SIDE ), file, RIGHT_SIDE );  
    }

    if( curSym == '}' && side == RIGHT_SIDE ) 
    {
        return 1;
    }

    return 1;
}

//-----------------------------------------------------------------------------

int SaveDiffDataRecursively( Node* node, const char* fileName )
{  
    ASSERT( node != NULL && fileName != NULL, 0 );    
    
    FILE* file = fopen( fileName, "w" );
    if(  !file  ) return 0;

    PrintPreorderNodes( node, file );

    fclose( file );
    return  1;
}

int SaveDiffData( Tree* tree, const char* fileName )
{
    ASSERT( tree     != NULL, 0 );
    ASSERT( fileName != NULL, 0 );
    
    SaveDiffDataRecursively( &tree->headNode, FileName );

    return 1;
}

//-----------------------------------------------------------------------------
