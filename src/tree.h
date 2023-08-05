#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "buffer.h"
typedef struct huffnode_st HuffNode;
struct huffnode_st {
    HuffNode *left;
    HuffNode *right;
    ssize_t key;
    char value;
};

int huffnode_compare(void *, void *);
void delete_hufftree(HuffNode *);
HuffNode *new_huffnode(HuffNode *, HuffNode *, ssize_t, char);
int isLeaf(HuffNode *);

#endif
