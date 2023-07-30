#include "huffman.h"
#include "queue.h"

void freqCount(Buffer *, size_t *);
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
    size_t freqs[256]; // frequency of every 8-bit integer value
    memset(freqs, 0, 256 * sizeof(size_t));
    freqCount(src, freqs);
    
    Tree *queue = NULL;
    for (int i = 0; i < 256; i++) {
        queue_push(queue, freqs[i], i);
    }
    while (queue_size(queue) > 1) {

    }
    return NULL;
}


Buffer *huffman_extract(Buffer *src);

void freqCount(Buffer *src, size_t *freqs)
{
    for (size_t i = 0; i < src->len; i++) {
        freqs[(int) src->data[i]]++;
    }
}
