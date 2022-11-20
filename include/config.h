#ifndef CONFIG_H
#define CONFIG_H

enum Types
{
    OP_TYPE, 
    VAL_TYPE, 
    VAR_TYPE
};

struct DiffNode
{
    int type; 

    // union in progress
    double dblValue;
    int    opValue;
    char*  varValue;
};

typedef DiffNode* TreeElem_t;

struct Node;

typedef Node* Elem_t; // For Stack    

const int MaxStrLen = 255;

#endif