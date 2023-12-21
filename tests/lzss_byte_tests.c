#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/lzss_common.h"
#include "../include/lzss_byte.h"
#include "../include/lzss_byte_private.h"
#include "../include/fileops.h"
#include "../include/ringbuffer.h"

START_TEST(testDecodeLZSSPayload)
{
    Buffer *buf = new_buffer();
    unsigned char *str = (unsigned char *)"I AM SAM. I AM SAM."; 
    buffer_append(buf, str, 19);
    Buffer *compressed = new_buffer();

    buffer_append(compressed, (unsigned char *)"I AM S", 6);
    // append token for "AM"stored in little-endian format
    writeByteToken(compressed, 4, 2);
    buffer_append(compressed, (unsigned char *)". ", 2);
    writeByteToken(compressed, 10, 9);
    BufferReader *reader = buffer_createReader(compressed);
    Buffer *decompressed = decodeLZSSPayloadByteLevel(reader);

    ck_assert_mem_eq(buf->data, decompressed->data, 19);
}
END_TEST

START_TEST(testEncodeDecodeLZSSPayload)
{
    Buffer *src = new_buffer();
    char *str = "jiopfwejfopfmwealfnn09v09a8g-afewjopf23rn;jfsaf";
    size_t len = strlen(str) + 1;
    buffer_append(src, (unsigned char *)str, len);
    Buffer *compressed = new_buffer();
    encodeLZSSPayloadByteLevel(src, compressed);
    BufferReader *reader = buffer_createReader(compressed);
    Buffer *result = decodeLZSSPayloadByteLevel(reader);
    ck_assert_mem_eq(result->data, src->data, len);
}
END_TEST

START_TEST(testEncodeDecodeLZSSPayload2)
{
    Buffer *src = new_buffer();
    char *str = "fjowipaejfeo902380-29qui3r-f0efci0-eareqw-r";
    size_t len = strlen(str) + 1;
    buffer_append(src, (unsigned char *)str, len);
    Buffer *compressed = new_buffer();
    encodeLZSSPayloadByteLevel(src, compressed);
    BufferReader *reader = buffer_createReader(compressed);
    Buffer *result = decodeLZSSPayloadByteLevel(reader);
    ck_assert_mem_eq(result->data, src->data, len);
}
END_TEST

START_TEST(testEncodeDecodeLZSSPayload3)
{
    Buffer *src = new_buffer();
    char *str = "";
    size_t len = strlen(str) + 1;
    buffer_append(src, (unsigned char *)str, len);
    Buffer *compressed = new_buffer();
    encodeLZSSPayloadByteLevel(src, compressed);
    BufferReader *reader = buffer_createReader(compressed);
    Buffer *result = decodeLZSSPayloadByteLevel(reader);
    ck_assert_mem_eq(result->data, src->data, len);
}
END_TEST

START_TEST(testCompressDecompressByte1)
{
    Buffer *file = readFile("samples/bliss-sample.bin");
    Buffer *compressed = lzss_byte_compress(file);
    Buffer *result = lzss_byte_extract(compressed);
    ck_assert_int_eq(buffer_equals(result, file), 1);
    delete_buffer(file);
    delete_buffer(compressed);
    delete_buffer(result);
}
END_TEST

START_TEST(testCompressDecompressByte2)
{
    Buffer *file = readFile("samples/linux-sample.bin");
    Buffer *compressed = lzss_byte_compress(file);
    Buffer *result = lzss_byte_extract(compressed);
    ck_assert_int_eq(buffer_equals(result, file), 1);
    delete_buffer(file);
    delete_buffer(compressed);
    delete_buffer(result);
}
END_TEST

START_TEST(testCompressDecompressByte3)
{
    Buffer *file = readFile("samples/loremipsum-100k.txt");
    Buffer *compressed = lzss_byte_compress(file);
    Buffer *result = lzss_byte_extract(compressed);
    ck_assert_int_eq(buffer_equals(result, file), 1);
    delete_buffer(file);
    delete_buffer(compressed);
    delete_buffer(result);
}
END_TEST

START_TEST(testCompressDecompressByte4)
{
    Buffer *file = readFile("samples/ff.bin");
    Buffer *compressed = lzss_byte_compress(file);
    Buffer *result = lzss_byte_extract(compressed);
    ck_assert_int_eq(buffer_equals(result, file), 1);
    delete_buffer(file);
    delete_buffer(compressed);
    delete_buffer(result);
}
END_TEST

Suite *lzss_suite(void)
{
    Suite *s;
    TCase *tc_unit;
    s = suite_create("LZSS-byte");
    tc_unit = tcase_create("Unit");
    tcase_add_test(tc_unit, testDecodeLZSSPayload);
    tcase_add_test(tc_unit, testEncodeDecodeLZSSPayload);
    tcase_add_test(tc_unit, testEncodeDecodeLZSSPayload2);
    tcase_add_test(tc_unit, testEncodeDecodeLZSSPayload3);
    TCase *tc_int = tcase_create("Integration");
    tcase_set_timeout(tc_int, 10);
    tcase_add_test(tc_int, testCompressDecompressByte1);
    tcase_add_test(tc_int, testCompressDecompressByte2);
    tcase_add_test(tc_int, testCompressDecompressByte3);
    tcase_add_test(tc_int, testCompressDecompressByte4);
    suite_add_tcase(s, tc_unit);
    suite_add_tcase(s, tc_int);

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
