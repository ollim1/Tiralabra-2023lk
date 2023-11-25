#ifndef HUFFNODE_H
#define HUFFNODE_H

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
int huffnode_isValid(HuffNode *node);
int huffnode_equals(HuffNode *node1, HuffNode *node2);
void huffnode_serialize(HuffNode *node, BitArray *dst);
HuffNode *huffnode_deserialize(BitArrayReader *src);
int huffnode_isLeaf(HuffNode *);

#endif
