#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/huffman.h"
#include "../src/huffman_private.h"
#include "../src/huffnode.h"
#include "../src/bitarray.h"

START_TEST(test_init_hufftree)
{
    HuffNode *nodeB = huffnode_createLeaf(3, 'b');
    HuffNode *nodeC = huffnode_createLeaf(7, 'c');
    HuffNode *nodeA = huffnode_createParent(nodeB, nodeC);

    ck_assert_ptr_eq(nodeA->left, nodeB);
    ck_assert_ptr_eq(nodeA->right, nodeC);
    ck_assert_ptr_eq(nodeB->left, NULL);
    ck_assert_ptr_eq(nodeB->right, NULL);
    ck_assert_ptr_eq(nodeC->left, NULL);
    ck_assert_ptr_eq(nodeC->right, NULL);
    ck_assert_int_eq(nodeA->key, 10);
    ck_assert_int_eq(nodeB->key, 3);
    ck_assert_int_eq(nodeC->key, 7);
    ck_assert_int_eq(nodeA->value, '\0');
    ck_assert_int_eq(nodeB->value, 'b');
    ck_assert_int_eq(nodeC->value, 'c');
    delete_huffnode(nodeA);
}
END_TEST

START_TEST(test_huffnode_createParent)
{
    HuffNode *nodeB = huffnode_createLeaf(3, 'b');
    HuffNode *nodeC = huffnode_createLeaf(7, 'c');
    HuffNode *nodeA = huffnode_createParent(nodeB, nodeC);

    ck_assert_ptr_eq(nodeA->left, nodeB);
    ck_assert_ptr_eq(nodeA->right, nodeC);
    ck_assert_ptr_eq(nodeB->left, NULL);
    ck_assert_ptr_eq(nodeB->right, NULL);
    ck_assert_ptr_eq(nodeC->left, NULL);
    ck_assert_ptr_eq(nodeC->right, NULL);
    ck_assert_int_eq(nodeA->key, 10);
    ck_assert_int_eq(nodeB->key, 3);
    ck_assert_int_eq(nodeC->key, 7);
    ck_assert_int_eq(nodeA->value, '\0');
    ck_assert_int_eq(nodeB->value, 'b');
    ck_assert_int_eq(nodeC->value, 'c');
    delete_huffnode(nodeA);
}
END_TEST

START_TEST(test_huffnode_compare)
{
    HuffNode *nodeB = huffnode_createLeaf(3, 'b');
    HuffNode *nodeC = huffnode_createLeaf(7, 'c');

    ck_assert_int_gt(huffnode_compare(nodeB, nodeC), 0);
    ck_assert_int_lt(huffnode_compare(nodeC, nodeB), 0);
    delete_huffnode(nodeB);
    delete_huffnode(nodeC);
}
END_TEST

START_TEST(test_huffnode_isLeaf)
{
    HuffNode *nodeB = huffnode_createLeaf(3, 'b');
    HuffNode *nodeC = huffnode_createLeaf(7, 'c');
    HuffNode *nodeA = huffnode_createParent(nodeB, nodeC);

    ck_assert_int_eq(huffnode_isLeaf(nodeA), 0);
    ck_assert_int_ne(huffnode_isLeaf(nodeB), 0);
    delete_huffnode(nodeA);
}
END_TEST

START_TEST(test_serialize_hufftree)
{
    HuffNode *b = huffnode_createLeaf(2, 'b');
    HuffNode *c = huffnode_createLeaf(1, 'c');
    HuffNode *a = huffnode_createLeaf(3, 'a');
    HuffNode *bc = huffnode_createParent(b, c);
    HuffNode *abc = huffnode_createParent(a, bc);

    BitArray *ba = new_bitarray();

    huffnode_serialize(abc, ba);
    // abc
    int result = bitarray_get(ba, 0);
    int expected = 0;
    ck_assert_int_eq(result, expected);
    // a
    result = bitarray_get(ba, 1);
    expected = 1;
    ck_assert_int_eq(result, expected);
    result = bitarray_getByte(ba, 2);
    expected = 'a';
    ck_assert_int_eq(result, expected);
    // bc
    result = bitarray_get(ba, 10);
    expected = 0;
    ck_assert_int_eq(result, expected);
    // b
    result = bitarray_get(ba, 11);
    expected = 1;
    ck_assert_int_eq(result, expected);
    result = bitarray_getByte(ba, 12);
    expected = 'b';
    ck_assert_int_eq(result, expected);
    // c
    result = bitarray_get(ba, 20);
    expected = 1;
    ck_assert_int_eq(result, expected);
    result = bitarray_getByte(ba, 21);
    expected = 'c';
    ck_assert_int_eq(result, expected);
    delete_bitarray(ba);
    delete_huffnode(abc);
}
END_TEST

START_TEST(test_deserialize_hufftree)
{
    HuffNode *b = huffnode_createLeaf(2, 'b');
    HuffNode *c = huffnode_createLeaf(1, 'c');
    HuffNode *a = huffnode_createLeaf(3, 'a');
    HuffNode *bc = huffnode_createParent(b, c);
    HuffNode *abc = huffnode_createParent(a, bc);

    BitArray *ba = new_bitarray();

    huffnode_serialize(abc, ba);
    BitArrayReader *br = bitarray_createReader(ba);
    HuffNode *deserialized = huffnode_deserialize(br);
    ck_assert_int_eq(huffnode_equals(abc, deserialized), 1);
    delete_bitarrayreader(br);
    delete_bitarray(ba);
    delete_huffnode(deserialized);
}
END_TEST

START_TEST(test_encodeLength)
{
    BitArray *ba = new_bitarray();

    encodeLength(ba, 0xe7489);
    BitArrayReader *br = bitarray_createReader(ba);

    int bit;
    unsigned char byte;
    bitarrayreader_readBit(br, &bit);
    ck_assert_int_eq(bit, 0);
    bitarrayreader_readByte(br, &byte);
    ck_assert_int_eq(byte, 0x89);
    bitarrayreader_readBit(br, &bit);
    ck_assert_int_eq(bit, 0);
    bitarrayreader_readByte(br, &byte);
    ck_assert_int_eq(byte, 0x74);
    bitarrayreader_readBit(br, &bit);
    ck_assert_int_eq(bit, 0);
    bitarrayreader_readByte(br, &byte);
    ck_assert_int_eq(byte, 0xe);
    bitarrayreader_readBit(br, &bit);
    ck_assert_int_eq(bit, 1);
    delete_bitarrayreader(br);
    delete_bitarray(ba);
}
END_TEST

START_TEST(test_decodeLength)
{
    BitArray *ba = new_bitarray();

    size_t expected = 0x7a1f9;
    size_t val = expected;

    while (val > 0) {
        bitarray_append(ba, 0);
        bitarray_appendByte(ba, val & 0xff);
        val >>= 8;
    }
    bitarray_append(ba, 1);
    BitArrayReader *br = bitarray_createReader(ba);
    size_t result = decodeLength(br);
    ck_assert_int_eq(result, expected);
}
END_TEST

START_TEST(test_buildHufftree)
{
    Buffer *buf = new_buffer();
    char *str = "abcdef";
    buffer_append(buf, str, strlen(str) + 1);

    HuffNode *tree = buildHufftree(buf);
    /*
     * Huffman trees may vary in structure based on priority queue implementation,
     * so difficult to test ouside checking completeness
     */
    ck_assert_int_eq(huffnode_isValid(tree), 1);
    delete_huffnode(tree);
    delete_buffer(buf);
}
END_TEST

START_TEST(test_cacheHuffcodes)
{
    HuffNode *b = huffnode_createLeaf(2, 'b');
    HuffNode *c = huffnode_createLeaf(1, 'c');
    HuffNode *a = huffnode_createLeaf(3, 'a');
    HuffNode *bc = huffnode_createParent(b, c);
    HuffNode *abc = huffnode_createParent(a, bc);

    BitArray *codes[MAX_LEAVES] = {NULL};
    char code[MAX_LEAVES + 1] = {'\0'};
    cacheHuffcodes(abc, codes, code, 0);

    BitArray *expected = new_bitarray_initl("0", 1);
    ck_assert_ptr_nonnull(codes['a']);
    ck_assert_int_eq(bitarray_equals(codes['a'], expected), 1); 
    delete_bitarray(expected);
    expected = new_bitarray_initl("10", 2);
    ck_assert_ptr_nonnull(codes['b']);
    ck_assert_int_eq(bitarray_equals(codes['b'], expected), 1); 
    delete_bitarray(expected);
    expected = new_bitarray_initl("11", 2);
    ck_assert_ptr_nonnull(codes['c']);
    ck_assert_int_eq(bitarray_equals(codes['c'], expected), 1); 
    delete_bitarray(expected);
}
END_TEST

START_TEST(test_encodePayload)
{
    BitArray *codes[MAX_LEAVES + 1] = {NULL};
    Buffer *src = new_buffer();
    buffer_append(src, (unsigned char *)"aaabbc", 6);
    codes['a'] = new_bitarray_initl("0", 1);
    codes['b'] = new_bitarray_initl("10", 2);
    codes['c'] = new_bitarray_initl("11", 2);
    BitArray *result = new_bitarray();
    encodePayload(src, result, codes);
    BitArray *expected = new_bitarray_initl("000101011", 9);
    ck_assert_int_eq(bitarray_equals(result, expected), 1);
    delete_bitarray(expected);
    delete_bitarray(result);
    for (int i = 0; i < MAX_LEAVES + 1; i++) {
        if (codes[i])
            delete_bitarray(codes[i]);
    }
    delete_buffer(src);
}
END_TEST

START_TEST(test_decodePayload)
{
    HuffNode *b = huffnode_createLeaf(2, 'b');
    HuffNode *c = huffnode_createLeaf(1, 'c');
    HuffNode *a = huffnode_createLeaf(3, 'a');
    HuffNode *bc = huffnode_createParent(b, c);
    HuffNode *abc = huffnode_createParent(a, bc);

    BitArray *src = new_bitarray_initl("000101011",9);
    BitArrayReader *reader = bitarray_createReader(src);
    Buffer *decoded = decodePayload(reader, abc, 6);
    char *result = (char *)decoded->data;
    ck_assert_int_eq(strncmp("aaabbc", result, 6), 0);
}
END_TEST

START_TEST(test_huffman_compress_decompress_1)
{
    // hello world
    Buffer *src = new_buffer();
    char *str1 = "Hello, world!";
    buffer_append(src, (unsigned char *)str1, strlen(str1) + 1);
    Buffer *compressed = huffman_compress(src);
    Buffer *result = huffman_extract(compressed);
    ck_assert_str_eq((char *)result->data, str1);
}
END_TEST

START_TEST(test_huffman_compress_decompress_2)
{
    // numbers
    Buffer *src = new_buffer();
    char *str1 = "20\n15\n10\n1\n-12";
    buffer_append(src, (unsigned char *)str1, strlen(str1) + 1);
    Buffer *compressed = huffman_compress(src);
    Buffer *result = huffman_extract(compressed);
    ck_assert_str_eq((char *)result->data, str1);
}
END_TEST

START_TEST(test_huffman_compress_decompress_3)
{
    // same character
    Buffer *src = new_buffer();
    char *str1 = "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa";
    buffer_append(src, (unsigned char *)str1, strlen(str1) + 1);
    Buffer *compressed = huffman_compress(src);
    Buffer *result = huffman_extract(compressed);
    ck_assert_str_eq((char *)result->data, str1);
}
END_TEST

START_TEST(test_huffman_compress_decompress_4)
{
    Buffer *src = new_buffer();
    int i;
    for (i = 1; i < 256; i++) {
        buffer_append(src, (unsigned char *)&i, 1);
    }
    i = 0;
    buffer_append(src, (unsigned char *)&i, 1);
    Buffer *compressed = huffman_compress(src);
    Buffer *result = huffman_extract(compressed);
    ck_assert_str_eq((char *)result->data, src->data);
}
END_TEST

START_TEST(test_huffman_compress_decompress_empty)
{
    Buffer *src = new_buffer();
    Buffer *compressed = huffman_compress(src);
    Buffer *result = huffman_extract(compressed);
    ck_assert_str_eq((char *)result->data, (char *)src->data);
}
END_TEST

Suite *hufftree_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("HuffTree");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_init_hufftree);
    tcase_add_test(tc_core, test_huffnode_createParent);
    tcase_add_test(tc_core, test_huffnode_compare);
    tcase_add_test(tc_core, test_huffnode_isLeaf);
    suite_add_tcase(s, tc_core);

    return s;
}

Suite *huffman_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("Huffman");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_serialize_hufftree);
    tcase_add_test(tc_core, test_deserialize_hufftree);
    tcase_add_test(tc_core, test_encodeLength);
    tcase_add_test(tc_core, test_decodeLength);
    tcase_add_test(tc_core, test_buildHufftree);
    tcase_add_test(tc_core, test_cacheHuffcodes);
    tcase_add_test(tc_core, test_encodePayload);
    tcase_add_test(tc_core, test_decodePayload);
    tcase_add_test(tc_core, test_huffman_compress_decompress_1);
    tcase_add_test(tc_core, test_huffman_compress_decompress_2);
    tcase_add_test(tc_core, test_huffman_compress_decompress_3);
    tcase_add_test(tc_core, test_huffman_compress_decompress_4);
    tcase_add_test(tc_core, test_huffman_compress_decompress_empty);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *hufftree_s;
    Suite *huffman_s;
    SRunner *sr;

    hufftree_s = hufftree_suite();
    huffman_s = huffman_suite();
    sr = srunner_create(hufftree_s);
    srunner_add_suite(sr, huffman_s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
