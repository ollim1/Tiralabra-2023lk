#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/lzss.h"
#include "../src/lzss_private.h"
#include "../src/ringbuffer.h"
#include "../src/bitarray.h"

START_TEST(testFindMatch1)
{
    RingBuffer *haystack = new_ringbuffer(1000);
    Buffer *needle = new_buffer();
    unsigned char *strA = (unsigned char *)"aybabtu";
    unsigned char *strB = (unsigned char *)"bab";
    ringbuffer_appendString(haystack, strA, 7);
    buffer_append(needle, strB, 3);

    int result = findMatch(haystack, needle);
    ck_assert_int_eq(result, 5);
}
END_TEST

START_TEST(testFindMatch2)
{
    RingBuffer *haystack = new_ringbuffer(30);
    Buffer *needle = new_buffer();
    char *strA = "I AM SAM. I AM SAM. I AM SAM. I AM SAM. I AM SAM. I AM SAM. I AM SAM. I AM SAM.";
    char *strB = "I AM SAM.";
    ringbuffer_appendString(haystack, (unsigned char *)strA, strlen(strA) + 1);
    buffer_append(needle, (unsigned char *)strB, strlen(strB) + 1);

    int result = findMatch(haystack, needle);
    ck_assert_int_eq(result, 10);
}
END_TEST

START_TEST(testFindMatch3)
{
    RingBuffer *haystack = new_ringbuffer(30);
    Buffer *needle = new_buffer();
    char *strA = "";
    char *strB = "I AM SAM.";
    ringbuffer_appendString(haystack, (unsigned char *)strA, strlen(strA) + 1);
    buffer_append(needle, (unsigned char *)strB, strlen(strB) + 1);

    int result = findMatch(haystack, needle);
    ck_assert_int_eq(result, -1);
}
END_TEST

START_TEST(testEncodeLZSSPayload)
{
    Buffer *buf = new_buffer();
    unsigned char *str = (unsigned char *)"I AM SAM. I AM SAM."; 
    buffer_append(buf, str, 19);
    BitArray *result = new_bitarray();
    encodeLZSSPayloadBitLevel(buf, result);
    BitArray *expected = new_bitarray();
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, 'I');
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, ' ');
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, 'A');
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, 'M');
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, ' ');
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, 'S');
    // append token for "AM"stored in little-endian format
    writeToken(expected, 4, 2);
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, '.');
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, ' ');
    writeToken(expected, 10, 9);

    ck_assert_str_eq(bitarray_toString(result), bitarray_toString(expected));
}
END_TEST

START_TEST(testDecodeLZSSPayload)
{
    Buffer *buf = new_buffer();
    unsigned char *str = (unsigned char *)"I AM SAM. I AM SAM."; 
    buffer_append(buf, str, 19);
    BitArray *compressed = new_bitarray();
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, 'I');
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, ' ');
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, 'A');
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, 'M');
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, ' ');
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, 'S');
    // append token for "AM"stored in little-endian format
    writeToken(compressed, 4, 2);
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, '.');
    bitarray_append(compressed, 0);
    bitarray_appendByte(compressed, ' ');
    writeToken(compressed, 10, 9);
    BitArrayReader *reader = bitarray_createReader(compressed);
    Buffer *decompressed = decodeLZSSPayloadBitLevel(reader, 19);

    ck_assert_mem_eq(buf->data, decompressed->data, 19);
}
END_TEST

START_TEST(testWriteToken)
{
    BitArray *result = new_bitarray();
    int distance_expected = 10;
    int length_expected = 10;
    writeToken(result, distance_expected, length_expected);
    // 1 10101010 00000000
    int bit = 0;
    unsigned char byte = 0;
    BitArrayReader *reader = bitarray_createReader(result);
    bitarrayreader_readBit(reader, &bit);
    ck_assert_int_eq(bit, 1);
    bitarrayreader_readByte(reader, &byte);
    ck_assert_int_eq(byte, 0);
    bitarrayreader_readByte(reader, &byte);
    ck_assert_int_eq(byte, 170);
}
END_TEST

START_TEST(testWriteToken2)
{
    BitArray *result = new_bitarray();
    int distance_expected = 10;
    int length_expected = 9;
    writeToken(result, distance_expected, length_expected);
    // 1 10101010 00000000
    int bit = 0;
    unsigned char byte = 0;
    BitArray *expected = new_bitarray();
    uint32_t val = (distance_expected << TOKEN_LENGTH_BITS);
    val |= length_expected;
    bitarray_append(expected, 1); // mark the beginning of a token
    bitarray_appendByte(expected, (val & (~0xff)) >> 8);
    bitarray_appendByte(expected, val & 0xff);

    ck_assert_str_eq(bitarray_toString(result), bitarray_toString(expected));
}
END_TEST

START_TEST(testReadWriteToken)
{
    for (int i = 0; i < WINDOW_SIZE; i += 11) {
        BitArray *ba = new_bitarray();
        int distance_expected = i;
        int length_expected = i % 15;
        writeToken(ba, distance_expected, length_expected);
        BitArrayReader *br = bitarray_createReader(ba);
        int a;
        bitarrayreader_readBit(br, &a);
        int distance_result = 0;
        int length_result = 0;
        readToken(br, &distance_result, &length_result);

        ck_assert_int_eq(distance_result, distance_expected);
        ck_assert_int_eq(length_result, length_expected);
        delete_bitarrayreader(br);
        delete_bitarray(ba);
    }
}
END_TEST

START_TEST(testEncodeDecodeLZSSPayload)
{
    Buffer *src = new_buffer();
    char *str = "jiopfwejfopfmwealfnn09v09a8g-afewjopf23rn;jfsaf";
    size_t len = strlen(str) + 1;
    buffer_append(src, (unsigned char *)str, len);
    BitArray *ba = new_bitarray();
    encodeLZSSPayloadBitLevel(src, ba);
    BitArrayReader *reader = bitarray_createReader(ba);
    Buffer *result = decodeLZSSPayloadBitLevel(reader, len);
    ck_assert_mem_eq(result->data, src->data, len);
}
END_TEST

START_TEST(testEncodeDecodeLZSSPayload2)
{
    Buffer *src = new_buffer();
    char *str = "fjowipaejfeo902380-29qui3r-f0efci0-eareqw-r";
    size_t len = strlen(str) + 1;
    buffer_append(src, (unsigned char *)str, len);
    BitArray *ba = new_bitarray();
    encodeLZSSPayloadBitLevel(src, ba);
    BitArrayReader *reader = bitarray_createReader(ba);
    Buffer *result = decodeLZSSPayloadBitLevel(reader, len);
    ck_assert_mem_eq(result->data, src->data, len);
}
END_TEST

START_TEST(testEncodeDecodeLZSSPayload3)
{
    Buffer *src = new_buffer();
    char *str = "";
    size_t len = strlen(str) + 1;
    buffer_append(src, (unsigned char *)str, len);
    BitArray *ba = new_bitarray();
    encodeLZSSPayloadBitLevel(src, ba);
    BitArrayReader *reader = bitarray_createReader(ba);
    Buffer *result = decodeLZSSPayloadBitLevel(reader, len);
    ck_assert_mem_eq(result->data, src->data, len);
}
END_TEST

Suite *lzss_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("LZSS");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, testFindMatch1);
    tcase_add_test(tc_core, testFindMatch2);
    tcase_add_test(tc_core, testFindMatch3);
    tcase_add_test(tc_core, testReadWriteToken);
    tcase_add_test(tc_core, testWriteToken);
    tcase_add_test(tc_core, testWriteToken2);
    tcase_add_test(tc_core, testEncodeLZSSPayload);
    tcase_add_test(tc_core, testDecodeLZSSPayload);
    tcase_add_test(tc_core, testEncodeDecodeLZSSPayload);
    tcase_add_test(tc_core, testEncodeDecodeLZSSPayload2);
    tcase_add_test(tc_core, testEncodeDecodeLZSSPayload3);
    suite_add_tcase(s, tc_core);

    return s;
}


int main(void)
{
    int number_failed;
    Suite *lzss_s;
    SRunner *sr;

    lzss_s = lzss_suite();
    sr = srunner_create(lzss_s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
