
#include "build_tree_rec.h"
#include "dsl.h"
#include "differentiator.h"
#include "assert.h"
#include "my_assert.h"
#include "tree.h"
#include "LOG.h"

#include <string.h>
#include <ctype.h>

//-----------------------------------------------------------------------------

Node* LoadDiffDataTree( const char* diffDataTree )
{
    ASSERT( diffDataTree != NULL, 0 );

    char* str = ( char* )diffDataTree;

    Node* node = GetG( &str );
    LinkNodeParents( node, NULL );

    return node;
}

//-----------------------------------------------------------------------------

Node* GetG( char** str )
{   
    Node* node = GetE( str );

	assert( **str == '\0' );

	return node;
}

//-----------------------------------------------------------------------------

Node* GetE( char** str )
{
	Node* node = GetT( str );

	while( **str == '+' || **str == '-' )
	{
		char op = **str;
		(*str)++;

		Node* nodeR = GetT( str );
		Node* nodeL = CopyNode( node );	

		if( op == '+' )
        {        
            node = ADD( nodeL, nodeR );
        }
		else
        {
			node = SUB( nodeL, nodeR );	
        }
	}

	return node;
}

//-----------------------------------------------------------------------------

Node* GetT( char** str )
{
	Node* node = GetPower( str );

	while( **str == '*' || **str == '/' )
	{
		char op = **str;
		(*str)++;

		Node* nodeR = GetPower( str );
		Node* nodeL = CopyNode( node );	

		if( op == '*' )
        {
			node = MUL( nodeL, nodeR );
        }
		else
        {
			node = DIV( nodeL, nodeR );
        }
	}

	return node;
}

//-----------------------------------------------------------------------------

Node* GetPower( char** str )
{
	Node* node = GetBracket( str );

	if( **str == '^' ) 
	{
		(*str)++;

		Node* nodeR = GetPower( str );
		Node* nodeL = CopyNode( node );	

		node = POW( nodeL, nodeR );
	}

	return node;
}

//-----------------------------------------------------------------------------

Node* GetBracket( char** str )
{	
	if( **str == '(' )
	{
		(*str)++;
		
		Node* node = GetE( str );
		
		assert( **str == ')' );
		(*str)++;

		return node;
	}
    
    return GetStrFunc( str );
}

//-----------------------------------------------------------------------------

Node* GetStrFunc( char** str )
{
	int op  = IsStrFunc( str );
	if( op != -1 )
	{
		Node* nodeR = GetBracket( str ); 
		
		switch( op )
		{
			case OP_SIN: return SIN( NULL, nodeR );
			case OP_COS: return COS( NULL, nodeR );
			case OP_LN:  return LN ( NULL, nodeR );
		}
	}
	
    return GetVar( str );
}

//-----------------------------------------------------------------------------

int IsStrFunc( char** str )
{
	if/* */( **str == 's' && *(*str + 1) == 'i' && *(*str + 2) == 'n' )
	{
		*str += 3;
		return OP_SIN;
	}
	else if( **str == 'c' && *(*str + 1) == 'o' && *(*str + 2) == 's' )
	{
		*str += 3;
		return OP_COS;
	}
	else if( **str == 'l' && *(*str + 1) == 'n' )
	{
		*str += 2;
		return OP_LN;
	}

	return -1;
}

//-----------------------------------------------------------------------------

Node* GetVar( char** str )
{	
	if( isalpha( **str ) )
	{        
		char* var    = ( char* )calloc( MaxStrLen, sizeof( char ) );
		char* oldStr = *str;
				
		int i = 0;	
		while( isalpha(**str) && i < MaxStrLen )
		{
			var[i] = **str;
			(*str)++;
			
            i++;	
		}
		
		assert( oldStr != *str );

		return CREATE_VAR_NODE( var );;
	}
	
	return GetNumber( str );
}

//-----------------------------------------------------------------------------

Node* GetNumber( char** str )
{
	double val   = 0;

    const char* sOld = *str;
	
    while( '0' <= **str && **str <= '9' )
    {
        val = val * 10 + (**str) - '0';
        (*str)++; 
    }

	assert( *str != sOld );
	
	return CREATE_VAL_NODE( val );
}

//-----------------------------------------------------------------------------