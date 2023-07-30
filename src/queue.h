#ifndef QUEUE_H
#define QUEUE_H

#include "buffer.h"
typedef struct node_st TreeNode;
struct node_st {
    TreeNode *left;
    TreeNode *right;
    size_t key;
    char value;
};
typedef struct tree_st {
    TreeNode *root;
} Tree;

void queue_push(Tree *, size_t, char);
TreeNode queue_pop(Tree *);
void delete_queue(Tree *);
size_t queue_size(Tree *);

#endif
