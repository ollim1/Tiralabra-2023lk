#include "tree.h"
int huffnode_compare(void *left, void *right)
{
    /*
     * comparator function for Huffman tree node
     * returns positive if the left operand is larger than the right operand
     */
    if (!left || !right)
        err_quit("comparing null pointers");
    HuffNode *a = (HuffNode *)left;
    HuffNode *b = (HuffNode *)right;

    return a->key - b->key;
}

void delete_hufftree(HuffNode *node)
{
    if (!node)
        err_quit("null pointer when trying to destroy tree");
    if (node->left)
        delete_hufftree(node->left);
    if (node->right)
        delete_hufftree(node->right);
    free(node);
}

HuffNode *new_huffnode(HuffNode *left, HuffNode *right, ssize_t key, char value)
{
    HuffNode *ret = malloc(sizeof(HuffNode));
    if (!ret)
        err_quit("failed to allocate memory for Huffman tree node");

    ret->left = left;
    ret->right = right;
    ret->key = key;
    ret->value = value;
    return ret;
}

HuffNode *huffnode_createParent(HuffNode *left, HuffNode *right)
{
    if (!left || !right)
        err_quit("can only create a parent node for two leaf nodes");
    HuffNode *ret = new_huffnode(left, right, left->key + right->key, '\0');
    return ret;
}

int huffnode_isLeaf(HuffNode *node)
{
    if (!node)
        err_quit("null pointer checking if node is a leaf");
    return node->left == NULL && node->right == NULL;
}
