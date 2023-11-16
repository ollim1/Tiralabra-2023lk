#include "tree.h"
#include "bitarray.h"
#include "error.h"

int huffnode_compare(void *left, void *right)
{
    /*
     * comparator function for Huffman tree node
     * returns positive if the left operand is smaller than the right operand
     */
    if (!left || !right)
        err_quit("comparing null pointers");
    HuffNode *a = (HuffNode *)left;
    HuffNode *b = (HuffNode *)right;

    return b->key - a->key;
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

HuffNode *huffnode_createLeaf(ssize_t key, char value)
{
    HuffNode *ret = new_huffnode(NULL, NULL, key, value);
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

void huffnode_serialize(HuffNode *node, BitArray *dst)
{
    /*
     * serialize tree into format:
     * if bit value is 1, the next 8 bits should contain a byte value
     * otherwise next bit will be the beginning bit of the left child, then the right child
     */
    if (!node || !dst)
        err_quit("null pointer on serialize");

    if (node->left == NULL) {
        bitarray_append(dst, 1);
        bitarray_appendByte(dst, node->value);
    } else {
        bitarray_append(dst, 0);
        huffnode_serialize(node->left, dst);
        huffnode_serialize(node->right, dst);
    }
}

HuffNode *huffnode_deserialize(BitArrayReader *src)
{
    if (!src)
        err_quit("null pointer on serialize");

    int bit;
    bitarrayreader_readBit(src, &bit);
    if (bit) {
        char byte;
        bitarrayreader_readByte(src, &byte);
        return new_huffnode(NULL, NULL, 0, byte);
    } else {
        HuffNode *left = huffnode_deserialize(src);
        HuffNode *right = huffnode_deserialize(src);
        return new_huffnode(left, right, 0, 0);
    }
}
