#include "codedtree.h"

void readTree(HuffNode *node, HuffCode *codes, unsigned *len, unsigned depth, char code[32]);
int huffcode_compare(void *a, void *b)
{
    /*
     * compare Huffman codes by decoded alphabetical value
     * returns positive if right value is bigger
     */
    HuffCode *left = (HuffCode *)a;
    HuffCode *right = (HuffCode *)b;
    return ((int) right->value) - ((int) left->value); 
}

BitArray *serialize_hufftree(HuffNode *src)
{
    if (!src)
        err_quit("null pointer when serializing hufftree");

    BitArray *ret = new_bitarray();
    // get tree leaves with their associated codes by depth first traversal
    HuffCode codes[256];
    unsigned len = 0;
    memset(&codes, 0, sizeof(codes));
    char tempCode[256];
    readTree(src, codes, &len, 0, tempCode);

    // sort codes by value
    PriorityQueue *sort_queue = new_queue(huffcode_compare);
    for (int i = 0; i < 256; i++) {
        if (codes[i].len > 0)
            queue_insert(sort_queue, (void *)&(codes[i]));
    }
    HuffCode *sorted[256];
    memset(&sorted, 0, sizeof(sorted));
    for (int i = 0; i < len; i++)
        sorted[i] = queue_pop(sort_queue);
    delete_queue(sort_queue);

    // write code lengths from sorted list to output
    bitarray_pad(ret, 256);
    for (int i = 0; i < len; i++)
        bitarray_setString(ret, &(sorted[i]->len), 8, (int) sorted[i]->value);
    
    // write traversal codes
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < sorted[i]->len; j++) {
            bitarray_append(ret, sorted[i]->code[j]);
        }
    }
    return ret;
}

void readTree(HuffNode *node, HuffCode *codes, unsigned *len, unsigned depth, char *code)
{
    /*
     * Descend Huffman tree depth first, mark path into code
     * Takes Huffman node, pointer to current array length, current depth and
     * pointer to array with the current code, backtracking style
     */
    if (!node)
        err_quit("null pointer when traversing huffman tree");
    if (*len > 255)
        err_quit("huffman code list overfilled while traversing");
    if (depth > 255)
        err_quit("depth limit exceeded");

    if (huffnode_isLeaf(node)) { // leaf node found, adding code to list
        codes[*len].value = node->value;
        memcpy(codes[*len].code, code, depth);
        codes[*len].len = depth;
        (*len) += 1;
    } else {
        code[depth] = 1;
        readTree(node->left, codes, len, depth+1, code); // left branch
        code[depth] = 0;
        readTree(node->right, codes, len, depth+1, code); // right branch
    }
}

unsigned deserialize_hufftree(BitArray *src, HuffNode *dst)
{
    return 0;   
}

