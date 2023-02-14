#ifndef BUILD_TREE_REC_H
#define BUILD_TREE_REC_H

struct Node;

Node* LoadDiffDataTree( const char* diffDataTree );

Node* GetG      ( char** str );
Node* GetE      ( char** str );
Node* GetT      ( char** str );
Node* GetPower  ( char** str );
Node* GetBracket( char** str );
Node* GetStrFunc( char** str );
Node* GetVar    ( char** str );
Node* GetNumber ( char** str );

int IsStrFunc( char** str );

#endif