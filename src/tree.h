#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "buffer.h"
typedef struct huffnode_st HuffTreeNode;
struct huffnode_st {
    HuffTreeNode *left;
    HuffTreeNode *right;
    ssize_t key;
    char value;
};

int huffnode_compare(void *, void *);
void delete_hufftree(HuffTreeNode *);
HuffTreeNode *new_huffnode(HuffTreeNode *, HuffTreeNode *, ssize_t, char);
int isLeaf(HuffTreeNode *);

#endif
