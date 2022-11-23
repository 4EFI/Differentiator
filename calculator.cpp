
#include <stdio.h>
#include <assert.h>

//-----------------------------------------------------------------------------

const char* S = "";

//-----------------------------------------------------------------------------

int GetG( const char* str );
int GetN();
int GetE();
int GetT();
int GetP();

//-----------------------------------------------------------------------------

int main()
{
    printf( "%d", GetG( "(100-90)/10" ) ); 
}

//-----------------------------------------------------------------------------

int GetG( const char* str )
{
    S = str;
    int val = GetE();

    assert( *S == '\0' );
    return val;
}

//-----------------------------------------------------------------------------

int GetN()
{  
    int val = 0;
    const char* sOld = S;

    while( '0' <= *S && *S <= '9' )
    {
        val = val * 10 + (*S) - '0';
        S++; 
    }

    assert( S != sOld );
    return val;
}

//-----------------------------------------------------------------------------

int GetE()
{
    int val = GetT();
    
    while( *S == '+' || *S == '-' )
    {
        char op = *S;
        S++;
        int val2 = GetT();

        if( op == '+' ) val += val2;
        else            val -= val2; 
    }
    
    return val;
}

//-----------------------------------------------------------------------------

int GetT()
{
    int val = GetP();

    while( *S == '*' || *S == '/' )
    {
        char op = *S;
        S++;
        int val2 = GetP();

        if( op == '*' ) val *= val2;
        else            val /= val2;
    }

    return val;
}

//-----------------------------------------------------------------------------

int GetP()
{
    int val = 0;

    if( *S == '(' )
    {
        S++;
        val = GetE();
        
        assert( *S == ')' );
        S++;
    }
    else
    {
        val = GetN();
    }

    return val;
}

//-----------------------------------------------------------------------------

