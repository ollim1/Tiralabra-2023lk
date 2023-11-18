#ifndef HUFFTREE_H
#define HUFFTREE_H

#include <sys/types.h>
#include "buffer.h"
#include "bitarray.h"
typedef struct huffnode_st HuffNode;
struct huffnode_st {
    HuffNode *left;
    HuffNode *right;
    ssize_t key;
    unsigned char value;
};

int huffnode_compare(void *, void *);
void delete_huffnode(HuffNode *);
HuffNode *new_huffnode(HuffNode *, HuffNode *, ssize_t, unsigned char);
HuffNode *huffnode_createLeaf(ssize_t key, unsigned char value);
HuffNode *huffnode_createParent(HuffNode *, HuffNode *);
void huffnode_serialize(HuffNode *node, BitArray *dst);
HuffNode *huffnode_deserialize(BitArrayReader *src);
int huffnode_isLeaf(HuffNode *);

#endif
