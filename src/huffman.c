#include "huffman.h"
#include "queue.h"
#include "tree.h"
#include "bitarray.h"

HuffNode *buildHufftree(Buffer *src);
void cacheHuffcodes(HuffNode *, BitArray **, BitArray *, int length);
void encodeData(Buffer *src, BitArray *dst, BitArray **codes);
void encodeLength(BitArray *ba, size_t val);
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
    // building tree
    HuffNode *tree = buildHufftree(src);

    // storing codes into a lookup table
    unsigned char lengths[MAX_LEAVES] = {0}; // lengths of coded huffman tree items
    BitArray *codes[MAX_LEAVES] = {NULL}; // Huffman coded characters
    BitArray *code = new_bitarray();
    bitarray_pad(code, MAX_LEAVES);
    cacheHuffcodes(tree, codes, code, 0);
    delete_bitarray(code);

    // encoding input data using Huffman coded characters
    BitArray *output = new_bitarray();
    // encode unpacked length
    encodeLength(output, src->len);
    // encode tree
    huffnode_serialize(tree, output);
    // encode payload
    encodeData(src, output, codes);

    Buffer *ret = bitarray_toBuffer(output);
    delete_bitarray(output);
    delete_huffnode(tree);
    for (int i = 0; i < MAX_LEAVES; i++) {
        if (codes[i])
            delete_bitarray(codes[i]);
    }
    return ret;
}

void encodeData(Buffer *src, BitArray *dst, BitArray **codes)
{
    if (!src || !dst || !codes)
        err_quit("null pointer in encodeData");
    
    for (size_t i = 0; i < src->len; i++) {
        BitArray *traversal = codes[src->data[i]];
        if (!traversal)
            err_quit("invalid input, traversal code not found");
        bitarray_concat(dst, traversal);
    }
}

void encodeLength(BitArray *ba, size_t val)
{
    /*
     * encode Huffman buffer length in bit packed format:
     * 1 bit for end marker, 8 bits for data
     * little endian order
     */
    while (val > 0) {
        bitarray_append(ba, 0);
        bitarray_appendByte(ba, val & 0xff);
        val >>= 8;
    }
    bitarray_append(ba, 1); // append end marker
}

size_t decodeLength(BitArrayReader *br)
{
    /*
     * decode Huffman buffer length
     */
    int end;
    
    if (bitarrayreader_readBit(br, &end) != 1)
        err_quit("failed to read bit in decodeLength");

    size_t ret = 0;
    int offset = 0;
    while (!end) {
        unsigned char byte;
        if (bitarrayreader_readByte(br, &byte) != 8)
            err_quit("failed to read byte in decodeLength");
        ret |= (byte << offset);
        offset += 8;
        if (bitarrayreader_readBit(br, &end) != 1)
            err_quit("failed to read bit in decodeLength");
    }
    return ret;
}

HuffNode *buildHufftree(Buffer *src)
{
    /*
     * Building Huffman tree:
     * - calculate frequencies of each symbol
     * - push frequency-value pairs into min heap as leaf nodes
     * - pop two nodes and push a new internal node pointing to the two leaf
     *   nodes into heap, smaller on the left
     */
    size_t freqs[MAX_LEAVES] = {0}; // frequency of every 8-bit integer value
    for (size_t i = 0; i < src->len; i++) {
        freqs[(int)src->data[i]]++;
    }

    PriorityQueue *queue = new_queue(huffnode_compare);
    for (int i = 0; i < MAX_LEAVES - 1; i++) {
        if (freqs[i] == 0)
            continue;
        HuffNode *new_leaf = huffnode_createLeaf(freqs[i], i);
        queue_insert(queue, new_leaf);
    }
    while (queue_size(queue) > 1) {
        HuffNode *a = queue_pop(queue);
        HuffNode *b = queue_pop(queue);
        HuffNode *parent = huffnode_createParent(a, b);
        queue_insert(queue, parent);
    }
    HuffNode *root = NULL;
    if (queue_size(queue) == 1)
        root = queue_pop(queue);
    delete_queue(queue);
    return root;
}

void cacheHuffcodes(HuffNode *node, BitArray **codes, BitArray *code, int depth)
{
    /*
     * cache Huffman tree nodes into easily searchable arrays for encoding
     * takes a preallocated BitArray of MAX_LEAVES length as traversals can be
     * MAX_LEAVES bits long
     */
    if (!node)
        return;

    if (huffnode_isLeaf(node)) {
        codes[node->value] = bitarray_copyl(code, depth);
        return;
    }

    cacheHuffcodes(node->left, codes, code, depth + 1);
    bitarray_set(code, 1, depth);
    cacheHuffcodes(node->right, codes, code, depth + 1);
    bitarray_set(code, 0, depth);
}

Buffer *huffman_extract(Buffer *src);
