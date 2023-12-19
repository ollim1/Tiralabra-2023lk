#include "../include/huffnode.h"
#include "../include/bitarray.h"
#include "../include/error.h"
#include "../include/ealloc.h"

int huffnode_compare(void *left, void *right)
{
    /*
     * comparator function for Huffman tree node
     * returns positive if the left operand is smaller than the right operand
     */
    if (!left || !right)
        err_quit("comparing null pointers");
    HuffNode *a = (HuffNode *) left;
    HuffNode *b = (HuffNode *) right;

    return b->key - a->key;
}

int huffnode_isValid(HuffNode *node)
{
    /*
     * check if tree is complete
     */
    if ((!node->left && node->right) || (node->left && !node->right))
        return 0;
    else if (huffnode_isLeaf(node))
        return 1;
    return huffnode_isValid(node->left) && huffnode_isValid(node->right);
}

int huffnode_equals(HuffNode *node1, HuffNode *node2)
{
    /*
     * check if tree at node1 is equal to tree at node2
     */
    if (!node1 || !node2) {
        fprintf(stderr, "tree asymmetry: left %s, %s", !node1 ? "is null" : "is not null",
                !node2 ? "is null" : "is not null");
        return 0;
    }

    if (huffnode_isLeaf(node1) && huffnode_isLeaf(node2)) {
        if (node1->value == node2->value) {
            return 1;
        } else {
            fprintf(stderr, "tree values different: left: %d, %d", (int) node1->value,
                    (int) node2->value);
            return 0;
        }

    } else if (huffnode_isLeaf(node1) || huffnode_isLeaf(node2)) {
        fprintf(stderr, "tree asymmetry: left %s, %s",
                huffnode_isLeaf(node1) == 1 ? "is leaf" : "is not leaf",
                huffnode_isLeaf(node2) == 1 ? "is leaf" : "is not leaf");
        return 0;
    }

    return huffnode_equals(node1->left, node2->left) && huffnode_equals(node1->right, node2->right);
}

void delete_huffnode(HuffNode *node)
{
    if (!node)
        err_quit("null pointer when trying to destroy tree");
    if (node->left)
        delete_huffnode(node->left);
    if (node->right)
        delete_huffnode(node->right);
    free(node);
}

HuffNode *new_huffnode(HuffNode *left, HuffNode *right, ssize_t key, unsigned char value)
{
    HuffNode *ret = mmalloc(sizeof(HuffNode));

    ret->left = left;
    ret->right = right;
    ret->key = key;
    ret->value = value;
    return ret;
}

HuffNode *huffnode_createLeaf(ssize_t key, unsigned char value)
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
        unsigned char byte;
        bitarrayreader_readByte(src, &byte);
        return huffnode_createLeaf(0, byte);
    } else {
        HuffNode *left = huffnode_deserialize(src);
        HuffNode *right = huffnode_deserialize(src);
        return huffnode_createParent(left, right);
    }
}
