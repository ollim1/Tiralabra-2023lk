#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "buffer.h"
#include "sys/types.h"
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
HuffNode *huffnode_createParent(HuffNode *, HuffNode *);
int huffnode_isLeaf(HuffNode *);

#endif
