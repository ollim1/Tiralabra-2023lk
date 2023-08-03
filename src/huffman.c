#include "huffman.h"
#include "tree.h"
#include "queue.h"

void freqCount(Buffer *, size_t *);
HuffTreeNode *build_hufftree(size_t *, ssize_t );
Buffer *huffman_compress(Buffer *src)
{
    /* 
     * compresses a Buffer using the Huffman algorithm.
     * 1. frequency counting:
     * 2. tree building
     * 3. character encoding
     * - using 8-bit coding size, the frequency table and thus the tree will
     *   contain at most 256 leaves and 255 internal nodes
     */
    size_t freqs[MAX_LEAVES]; // frequency of every 8-bit integer value
    memset(freqs, 0, MAX_LEAVES * sizeof(size_t));
    freqCount(src, freqs);
    
    build_hufftree(freqs, MAX_LEAVES);
}

HuffTreeNode *build_hufftree(size_t *freqs, ssize_t len)
{
    PriorityQueue *queue = new_queue(huffnode_compare);
    for (int i = 0; i < MAX_LEAVES-1; i++) {
        HuffTreeNode *new_leaf = new_huffnode(NULL, NULL, freqs[i], i);
        queue_insert(queue, new_leaf);
    }
    while (queue_size(queue) > 1) {
        HuffTreeNode *a = queue_pop(queue);
        HuffTreeNode *b = queue_pop(queue);
        HuffTreeNode *parent = new_huffnode(a, b, a->key + b->key, -1);
        queue_insert(queue, parent);
    }
    HuffTreeNode *root = queue_pop(queue);
    delete_queue(queue);
    return root;
}

Buffer *huffman_extract(Buffer *src);

void freqCount(Buffer *src, size_t *freqs)
{
    for (size_t i = 0; i < src->len; i++) {
        freqs[(int) src->data[i]]++;
    }
}
