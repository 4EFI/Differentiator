#ifndef DSL_H
#define DSL_H

// Right/Left exists
#define IS_R_EXISTS ( node->right == NULL ? 0 : 1 )
#define IS_L_EXISTS ( node->left  == NULL ? 0 : 1 )

// Is right/left node
#define IS_R ( node->parent && node->parent->right == node )
#define IS_L ( node->parent && node->parent->left  == node )

#define IS_OP  ( node->value != NULL && node->value->type == Types::OP_TYPE  ? 1 : 0 )
#define IS_VAL ( node->value != NULL && node->value->type == Types::VAL_TYPE ? 1 : 0 )
#define IS_VAR ( node->value != NULL && node->value->type == Types::VAR_TYPE ? 1 : 0 )

// Right/Left node type
#define IS_R_VAL ( IS_R_EXISTS && node->right->value->type == Types::VAL_TYPE ? 1 : 0 )
#define IS_L_VAL ( IS_L_EXISTS && node->left ->value->type == Types::VAL_TYPE ? 1 : 0 )

#define IS_R_VAR ( IS_R_EXISTS && node->right->value->type == Types::VAR_TYPE ? 1 : 0 )
#define IS_L_VAR ( IS_L_EXISTS && node->left ->value->type == Types::VAR_TYPE ? 1 : 0 )

#define IS_R_OP ( IS_R_EXISTS && node->right->value->type == Types::OP_TYPE ? 1 : 0 )
#define IS_L_OP ( IS_L_EXISTS && node->left ->value->type == Types::OP_TYPE ? 1 : 0 )

// Right/left node dblValue
#define R_VAL node->right->value->dblValue
#define L_VAL node->left ->value->dblValue 

// Right/left node varValue
#define R_VAR node->right->value->varValue
#define L_VAR node->left ->value->varValue 

#define CREATE_VAL_NODE( NUM ) CreateNode( VAL_TYPE, NUM, -1 ) 
#define CREATE_VAR_NODE( VAR ) CreateNode( VAR_TYPE, 0,   -1, (char*)VAR )

#define CREATE_OP_NODE( OP, L, R ) CreateNode( OP_TYPE, 0, OP, NULL, L, R )

#define DL DifferentiateNode( node->left,  varName )
#define DR DifferentiateNode( node->right, varName )

#define CL CopyNode( node->left  )
#define CR CopyNode( node->right )

// +
#define ADD( L, R ) CREATE_OP_NODE( OP_ADD, L, R )
// -
#define SUB( L, R ) CREATE_OP_NODE( OP_SUB, L, R )
// *
#define MUL( L, R ) CREATE_OP_NODE( OP_MUL, L, R )
// /
#define DIV( L, R ) CREATE_OP_NODE( OP_DIV, L, R )
// sin
#define SIN( L, R ) CREATE_OP_NODE( OP_SIN, L, R )
// cos
#define COS( L, R ) CREATE_OP_NODE( OP_COS, L, R )
// ^
#define POW( L, R ) CREATE_OP_NODE( OP_DEG, L, R )
// ln
#define LN(  L, R ) CREATE_OP_NODE( OP_LN,  L, R ) 

#endif