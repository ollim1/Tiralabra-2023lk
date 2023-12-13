#include "../include/huffman.h"
#include "../include/huffman_private.h"
#include "../include/queue.h"
#include "../include/huffnode.h"
#include "../include/bitarray.h"

/**
 * compresses a Buffer using the Huffman algorithm.
 * 1. frequency counting:
 * 2. tree building and encoding
 *      - encode tree using depth first recursion:
 *      b:(cccccccc) where b is the leaf bit and c are character bits
 * 3. character encoding
 * @param src the source buffer to compress
 * @return compressed version of the given buffer
 */
Buffer *huffman_compress(Buffer *src)
{
    // building tree
    if (!src)
        err_quit("null pointer in huffman_compress");
    if (src->len == 0) {
        fputs("file is empty, skipping compression\n", stderr);
        return src;
    }
    HuffNode *tree = buildHufftree(src);

    // storing codes into a lookup table
    BitArray *codes[MAX_LEAVES] = {NULL}; // Huffman coded characters
    char code[MAX_LEAVES + 1] = {'\0'};
    cacheHuffcodes(tree, codes, code, 0);

    // encoding input data using Huffman coded characters
    BitArray *output = new_bitarray();
    // encode unpacked length
    bitarray_writeInteger(output, src->len);
    // encode tree
    huffnode_serialize(tree, output);
    fprintf(stderr, "size of serialized Huffman tree: %lu bits (%lu bytes)\n", output->len, output->len / 8 + 1);
    // encode payload
    encodeHuffmanPayload(src, output, codes);

    Buffer *ret = bitarray_toBuffer(output);
    delete_bitarray(output);
    delete_huffnode(tree);
    for (int i = 0; i < MAX_LEAVES; i++) {
        if (codes[i])
            delete_bitarray(codes[i]);
    }
    return ret;
}

/**
 * Encode compressed payload. Uses a code table to fetch codes matching byte values.
 * @param the source buffer to encode
 * @param dst the destination BitArray to which to write the compressed output
 * @param codes the pointer to the Huffman code table, containing tree paths
 */
void encodeHuffmanPayload(Buffer *src, BitArray *dst, BitArray **codes)
{
    if (!src || !dst || !codes)
        err_quit("null pointer in encodeHuffmanPayload");
    
    for (size_t i = 0; i < src->len; i++) {
        BitArray *traversal = codes[src->data[i]];
        if (!traversal)
            err_quit("invalid input, traversal code not found");
        if (traversal->len > 0)
            bitarray_concat(dst, traversal);
    }
}

/**
 * Building Huffman tree:
 * - calculate frequencies of each symbol
 * - push frequency-value pairs into min heap as leaf nodes
 * - pop two nodes and push a new internal node pointing to the two leaf
 *   nodes into heap, smaller on the left
 * @params src the source Buffer
 * @return the Huffman tree built using the Buffer
 */
HuffNode *buildHufftree(Buffer *src)
{
    size_t freqs[MAX_LEAVES] = {0}; // frequency of every 8-bit integer value
    for (size_t i = 0; i < src->len; i++) {
        freqs[(int)src->data[i]]++;
    }

    PriorityQueue *queue = new_queue(huffnode_compare);
    for (int i = 0; i < MAX_LEAVES; i++) {
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

/**
 * Cache Huffman tree nodes into a lookup table using a depth-first search.
 * @param node the Huffman tree to traverse
 * @param codes the destination code table
 * @param code string containing current code in '0' and '1' char literals
 * @param depth current depth
 */
void cacheHuffcodes(HuffNode *node, BitArray **codes, char *code, int depth)
{
    if (!node)
        return;

    if (huffnode_isLeaf(node)) {
        codes[node->value] = new_bitarray_fromStringl(code, depth);
        return;
    }

    code[depth] = '0';
    cacheHuffcodes(node->left, codes, code, depth + 1);
    code[depth] = '1';
    cacheHuffcodes(node->right, codes, code, depth + 1);
}

/**
 * Decode Huffman payload. Traverses Huffman tree according to bit values from
 * Huffman encoded input.
 * @param reader BitArrayReader, reused from earlier steps
 * @param tree Huffman tree to use for decoding
 * @param decoded_length the original size of the uncompressed file in bytes
 * @return decoded data
 */
Buffer *decodeHuffmanPayload(BitArrayReader *reader, HuffNode *tree, size_t decoded_length)
{
    if (!reader || !tree)
        err_quit("null pointer when decoding data");
    size_t total = 0;
    int bit;
    Buffer *output = new_buffer();
    HuffNode *node = tree;
    while (total < decoded_length) {
        if (huffnode_isLeaf(node)) {
            buffer_append(output, &(node->value), 1);
            node = tree;
            total++;
        } else {
            if (bitarrayreader_readBit(reader, &bit) < 1)
                err_quit("unexpected end of file while reading payload");
            if (bit == 1)
                node = node->right;
            else
                node = node->left;
        }
    }
    return output;
}

/**
 * Decompress Huffman compressed Buffer.
 * @param src compressed input
 * @return decompressed Buffer
 */
Buffer *huffman_extract(Buffer *src)
{
    if (src->len == 0) {
        fputs("file is empty, skipping extraction\n", stderr);
        return src;
    }
    // wrap Buffer in BitArray
    BitArray *data = bitarray_fromBuffer(src);
    // create reader for BitArray
    BitArrayReader *reader = bitarray_createReader(data);
    // decode uncompressed length
    size_t decoded_length = bitarrayreader_readInteger(reader);
    // deserialize tree
    HuffNode *tree = huffnode_deserialize(reader);
    if (!huffnode_isValid(tree))
        err_quit("failed to read header");
    // decode Huffman coded payload
    Buffer *ret = decodeHuffmanPayload(reader, tree, decoded_length);
    return ret;
}
