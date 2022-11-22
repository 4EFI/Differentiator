#ifndef DSL_H
#define DSL_H

// Right/Left node type
#define IS_R_VAL node->right->value->type == Types::VAL_TYPE ? 1 : 0
#define IS_L_VAL node->left ->value->type == Types::VAL_TYPE ? 1 : 0

#define IS_R_OP node->right->value->type == Types::OP_TYPE ? 1 : 0
#define IS_L_OP node->left ->value->type == Types::OP_TYPE ? 1 : 0

// Right/left node dblValue
#define R_VAL node->right->value->dblValue
#define L_VAL node->left ->value->dblValue

#define COUNT( OP ) L_VAL OP R_VAL 

#define CREATE_VAL_NODE( NUM ) CreateNode( VAL_TYPE, NUM, -1, NULL, NULL, NULL) 
#define CREATE_VAR_NODE( VAR ) CreateNode( VAR_TYPE, 0,   -1, #VAR, NULL, NULL)

#define DL DifferentiateNode( node->left  )
#define DR DifferentiateNode( node->right )

#define CL CopyNode( node->left  )
#define CR CopyNode( node->right )

// +
#define ADD( L, R ) CreateNode( OP_TYPE, 0, OP_ADD, NULL, L, R )
// -
#define SUB( L, R ) CreateNode( OP_TYPE, 0, OP_SUB, NULL, L, R )
// *
#define MUL( L, R ) CreateNode( OP_TYPE, 0, OP_MUL, NULL, L, R )
// /
#define DIV( L, R ) CreateNode( OP_TYPE, 0, OP_DIV, NULL, L, R )
// sin
#define SIN( L, R ) CreateNode( OP_TYPE, 0, OP_SIN, NULL, L, R )

#define COS(L, R) CreateNode(OP_TYPE, 0, OP_COS, NULL, L, R)
#define SQRT(L, R) CreateNode(OP_TYPE, 0, OP_SQRT, NULL, L, R)
#define POWER(L, R) CreateNode(OP_TYPE, 0, OP_POWER, NULL, L, R)
#define LN(L,R) CreateNode(OP_TYPE, 0, OP_LN, NULL, L, R)
#define CH(L,R) CreateNode(OP_TYPE, 0, OP_CH, NULL, L, R)
#define SH(L, R) CreateNode(OP_TYPE, 0, OP_SH, NULL, L, R)
#define IS_OP(oper) (node->type == OP_TYPE) && (node->optype == oper)
#define IS_LVAL(value) (node->leftchild->type == NUM_TYPE) && (compare(node->leftchild->val, value) == 0)
#define IS_RVAL(value) (node->rightchild->type == NUM_TYPE) && (compare(node->rightchild->val, value) == 0)

#endif