#include "huffman.h"
#include "queue.h"
#include "tree.h"
#include "bitarray.h"

void freqCount(Buffer *, size_t *);
HuffNode *build_hufftree(size_t *, size_t);
void cache_huffcodes(HuffNode *, char *, char *, int, int);
Buffer *huffman_compress(Buffer *src)
{
    /*
     * compresses a Buffer using the Huffman algorithm.
     * 1. frequency counting:
     * 2. tree building and encoding
     *      - encode tree using depth first recursion:
     *      b:(cccccccc) where b is the leaf bit and c are character bits
     * 3. character encoding
     */
    size_t freqs[MAX_LEAVES]; // frequency of every 8-bit integer value
    memset(freqs, 0, MAX_LEAVES * sizeof(size_t));
    freqCount(src, freqs);

    // building tree
    HuffNode *tree = build_hufftree(freqs, MAX_LEAVES);

    // caching codes for faster encoding
    char lengths[256] = {0}; // lengths of coded huffman tree items
    char codes[256] = {0}; // Huffman coded characters
    cache_huffcodes(tree, lengths, codes, 0, 0);

    // encoding input data using Huffman coded characters
    BitArray *output = new_bitarray();
    // first encode tree
    huffnode_serialize(tree, output);
    for (size_t i = 0; i < src->len; i++) {
        int src_byte = src->data[i];
        bitarray_appendString(output, &codes[src_byte], lengths[src_byte]); 
    }

    /*
     * Serializing Huffman tree
     */
    return NULL;
}

HuffNode *build_hufftree(size_t *freqs, size_t len)
{
    /*
     * Building Huffman tree:
     * - push leaf nodes into min heap
     * - pop two nodes and push a new internal node pointing to the two leaf nodes into heap, smaller on the left
     * - 
     */
    PriorityQueue *queue = new_queue(huffnode_compare);
    for (int i = 0; i < MAX_LEAVES - 1; i++) {
        HuffNode *new_leaf = huffnode_createLeaf(freqs[i], i);
        queue_insert(queue, new_leaf);
    }
    while (queue_size(queue) > 1) {
        HuffNode *a = queue_pop(queue);
        HuffNode *b = queue_pop(queue);
        HuffNode *parent = huffnode_createParent(a, b);
        queue_insert(queue, parent);
    }
    HuffNode *root = queue_pop(queue);
    delete_queue(queue);
    return root;
}

void cache_huffcodes(HuffNode *node, char *lengths, char *codes, int depth, int code)
{
    /*
     * cache Huffman tree nodes into easily searchable arrays for encoding
     */
    if (!node)
        return;

    if (huffnode_isLeaf(node)) {
        lengths[(int) node->value] = depth + 1;
        codes[(int) node->value] = code;
        return;
    }

    cache_huffcodes(node->left, lengths, codes, depth + 1, code);
    cache_huffcodes(node->right, lengths, codes, depth + 1, code | (1 << depth));
}

Buffer *huffman_extract(Buffer *src);

void freqCount(Buffer *src, size_t *freqs)
{
    for (size_t i = 0; i < src->len; i++) {
        freqs[(int)src->data[i]]++;
    }
}
