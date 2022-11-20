#ifndef CONFIG_H
#define CONFIG_H

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