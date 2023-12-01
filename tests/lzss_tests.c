#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/lzss.h"
#include "../src/lzss_private.h"
#include "../src/ringbuffer.h"
#include "../src/bitarray.h"

START_TEST(testFindMatch)
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
    bitarray_append(expected, 1);
    int token = (4 << TOKEN_LENGTH_BITS) | 2;
    bitarray_appendByte(expected, token & 0xff);
    bitarray_appendByte(expected, token >> 8);
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, '.');
    bitarray_append(expected, 0);
    bitarray_appendByte(expected, ' ');
    bitarray_append(expected, 1);
    token = (10 << TOKEN_LENGTH_BITS) | 10;
    bitarray_appendByte(expected, token & 0xff);
    bitarray_appendByte(expected, token >> 8);

    ck_assert_int_eq(bitarray_equals(result, expected), 0);
}
END_TEST

START_TEST(testDecodeLZSSPayload)
{
    Buffer *buf = new_buffer();
    unsigned char *str = (unsigned char *)"I AM SAM. I AM SAM."; 
    buffer_append(buf, str, 19);
    BitArray *compressed = new_bitarray();
    encodeLZSSPayloadBitLevel(buf, compressed);
    BitArrayReader *reader = bitarray_createReader(compressed);
    Buffer *decompressed = decodeLZSSPayloadBitLevel(reader, 19);

    ck_assert_mem_eq(buf->data, decompressed->data, 19);
}
END_TEST

Suite *lzss_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("LZSS");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, testFindMatch);
    tcase_add_test(tc_core, testEncodeLZSSPayload);
    tcase_add_test(tc_core, testDecodeLZSSPayload);
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
