#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bitarray.h"
#include "../src/ringbuffer.h"
#include "../src/buffer.h"
#include "../src/queue.h"

/*
 * Tests for utility libraries
 */

START_TEST(test_buffer_init)
{
    Buffer *buf = new_buffer();
    ck_assert_ptr_nonnull(buf);
    ck_assert_ptr_nonnull(buf->data);
    ck_assert_int_eq(buf->len, 0);
    delete_buffer(buf);
}
END_TEST

START_TEST(test_buffer_stores_string)
{
    Buffer *buf = new_buffer();
    char *testStr = "foo";
    size_t len = strlen(testStr) + 1;
    buffer_append(buf, testStr, len);
    ck_assert_str_eq(buf->data, testStr);
    delete_buffer(buf);
}
END_TEST

START_TEST(test_buffer_append)
{
    Buffer *buf = new_buffer();
    char *testStr = "foo";
    buffer_append(buf, testStr, 3); // leaving out null pointer
    buffer_append(buf, testStr, 4);
    ck_assert_str_eq(buf->data, "foofoo");
    ck_assert_int_eq(buf->len, 7);
    delete_buffer(buf);
}
END_TEST

START_TEST(test_buffer_pad)
{
    Buffer *buf = new_buffer();
    buffer_pad(buf, BUFSIZE * 2);
    ck_assert_int_eq(buf->len, BUFSIZE * 2);
    delete_buffer(buf);
}
END_TEST

START_TEST(test_buffer_expands)
{
    Buffer *buf = new_buffer();
    char testBuf[BUFSIZE * 2];
    buffer_append(buf, testBuf, BUFSIZE * 2); // leaving out null pointer
    ck_assert_int_gt(buf->len, BUFSIZE);
    ck_assert_int_gt(buf->len, BUFSIZE);
    delete_buffer(buf);
}
END_TEST

Suite *buffer_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("Buffer");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_buffer_init);
    tcase_add_test(tc_core, test_buffer_stores_string);
    tcase_add_test(tc_core, test_buffer_append);
    tcase_add_test(tc_core, test_buffer_pad);
    tcase_add_test(tc_core, test_buffer_expands);
    suite_add_tcase(s, tc_core);

    return s;
}

START_TEST(test_ringbuffer_init)
{
    RingBuffer *buf = new_ringbuffer(1000);
    ck_assert_ptr_nonnull(buf);
    ck_assert_ptr_nonnull(buf->data);
    ck_assert_int_eq(buf->len, 0);
    ck_assert_int_eq(buf->size, 1000);
    delete_ringbuffer(buf);
}
END_TEST

START_TEST(test_ringbuffer_append_get)
{
    RingBuffer *buf = new_ringbuffer(5);
    ringbuffer_appendString(buf, (unsigned char *)"aybabtu", 7);
    ck_assert_mem_eq(buf->data, (unsigned char *)"tubab", 5);
    ck_assert_int_eq(ringbuffer_get(buf, 0), 'b');
    ck_assert_int_eq(ringbuffer_getRev(buf, 0), 'u');
    delete_ringbuffer(buf);
}
END_TEST

Suite *ringbuffer_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("RingBuffer");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_ringbuffer_init);
    tcase_add_test(tc_core, test_ringbuffer_append_get);
    suite_add_tcase(s, tc_core);

    return s;
}

int test_comparator(void *a, void *b)
{
    int left = *(int *)a;
    int right = *(int *)b;
    if (left < right)
        return -1;
    else if (left > right)
        return 1;
    else
        return 0;
}

START_TEST(test_queue_init)
{
    PriorityQueue *queue = new_queue(test_comparator);

    ck_assert_ptr_nonnull(queue);
    delete_queue(queue);
}
END_TEST

START_TEST(test_queue_insert_peek)
{
    PriorityQueue *queue = new_queue(test_comparator);

    int a[5] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++)
        queue_insert(queue, &(a[i]));
    ck_assert_int_gt(*(int *)queue_peek(queue), 0);

    delete_queue(queue);
}
END_TEST

START_TEST(test_queue_pop)
{
    PriorityQueue *queue = new_queue(test_comparator);

    int a[5] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++)
        queue_insert(queue, &(a[i]));
    for (int i = 0; i < 5; i++)
        queue_pop(queue);
    ck_assert_int_eq(queue_size(queue), 0);

    delete_queue(queue);
}
END_TEST

START_TEST(test_queue_is_sorted)
{
    PriorityQueue *queue = new_queue(test_comparator);

    int a[5] = {15, 20, -12, 10, 1};
    for (int i = 0; i < 5; i++)
        queue_insert(queue, &(a[i]));
    int sorted[5] = {20, 15, 10, 1, -12};
    for (int i = 0; i < 5; i++) {
        int temp = *(int *)queue_pop(queue);
        ck_assert_int_eq(temp, sorted[i]);
    }
    delete_queue(queue);
}
END_TEST

Suite *priorityqueue_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("PriorityQueue");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_queue_init);
    tcase_add_test(tc_core, test_queue_insert_peek);
    tcase_add_test(tc_core, test_queue_pop);
    tcase_add_test(tc_core, test_queue_is_sorted);
    suite_add_tcase(s, tc_core);

    return s;
}

START_TEST(test_bitarray_init)
{
    BitArray *ba = new_bitarray();

    ck_assert_ptr_nonnull(ba);
    ck_assert_ptr_nonnull(ba->data);
    ck_assert_int_eq(ba->len, 0);

    delete_bitarray(ba);
}
END_TEST

START_TEST(test_bitarray_concat)
{
    BitArray *a = new_bitarray_fromStringl("01011", 5);
    BitArray *b = new_bitarray_fromStringl("10011", 5);
    BitArray *ab = new_bitarray_fromStringl("0101110011", 10);

    bitarray_concat(a, b);
    ck_assert_int_eq(bitarray_equals(a, ab), 1);
    delete_bitarray(a);
    delete_bitarray(b);
    delete_bitarray(ab);
}
END_TEST

START_TEST(test_bitarray_equals)
{
    BitArray *a = new_bitarray_fromStringl("01011", 5);
    BitArray *b = new_bitarray_fromStringl("10011", 5);

    bitarray_concat(a, b);
    ck_assert_int_eq(bitarray_equals(a, a), 1);
    ck_assert_int_eq(bitarray_equals(a, b), 0);
    delete_bitarray(a);
    delete_bitarray(b);
}
END_TEST

START_TEST(test_bitarray_writeInteger)
{
    BitArray *ba = new_bitarray();

    bitarray_writeInteger(ba, 0xe7489);
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

START_TEST(test_bitarrayreader_readInteger)
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
    size_t result = bitarrayreader_readInteger(br);
    ck_assert_int_eq(result, expected);
}
END_TEST

START_TEST(test_bitarray_set_get)
{
    BitArray *ba = new_bitarray();

    bitarray_pad(ba, 100);
    bitarray_set(ba, 1, 0);
    bitarray_set(ba, 1, 14);
    bitarray_set(ba, 0, 15);
    bitarray_set(ba, 1, 16);

    ck_assert_int_eq(bitarray_get(ba, 0), 1);
    ck_assert_int_eq(bitarray_get(ba, 14), 1);
    ck_assert_int_eq(bitarray_get(ba, 15), 0);
    ck_assert_int_eq(bitarray_get(ba, 16), 1);

    delete_bitarray(ba);
}
END_TEST

START_TEST(test_bitarray_append)
{
    BitArray *ba = new_bitarray();

    bitarray_append(ba, 1);
    bitarray_append(ba, 0);
    bitarray_append(ba, 1);
    bitarray_append(ba, 0);

    ck_assert_int_eq(bitarray_get(ba, 0), 1);
    ck_assert_int_eq(bitarray_get(ba, 1), 0);
    ck_assert_int_eq(bitarray_get(ba, 2), 1);
    ck_assert_int_eq(bitarray_get(ba, 3), 0);

    delete_bitarray(ba);
}
END_TEST

START_TEST(test_bitarray_appendstring)
{
    BitArray *ba = new_bitarray();

    char *test = "abc";
    bitarray_appendString(ba, test, 32);

    for (int i = 0; i < 32; i++) {
        int result = bitarray_get(ba, i);
        int expected = (test[i / 8] & (1 << (i % 8))) > 0;
        ck_assert_int_eq(result, expected);
    }

    delete_bitarray(ba);
}
END_TEST

START_TEST(test_bitarray_set_get_byte)
{
    BitArray *ba = new_bitarray();

    bitarray_appendByte(ba, 'a');

    ck_assert_int_eq(ba->len, 8);
    ck_assert_int_eq(bitarray_getByte(ba, 0), 'a');

    delete_bitarray(ba);
}
END_TEST

START_TEST(test_bitarray_toBuffer)
{
    BitArray *ba = new_bitarray();
    bitarray_appendString(ba, (unsigned char *) "abc", 32);
    Buffer *result = bitarray_toBuffer(ba);
    ck_assert_int_eq(strncmp((unsigned char *)result->data, "abc", 4), 0);
}
END_TEST

START_TEST(test_bitarray_appendZeroLength)
{
    BitArray *ba = new_bitarray();

    char *test = "";
    bitarray_appendString(ba, test, 0);
    ck_assert_int_eq(ba->len, 0);

    delete_bitarray(ba);
}
END_TEST

Suite *bitarray_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("BitArray");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_bitarray_init);
    tcase_add_test(tc_core, test_bitarray_set_get);
    tcase_add_test(tc_core, test_bitarray_append);
    tcase_add_test(tc_core, test_bitarray_appendstring);
    tcase_add_test(tc_core, test_bitarray_appendZeroLength);
    tcase_add_test(tc_core, test_bitarray_set_get_byte);
    tcase_add_test(tc_core, test_bitarray_writeInteger);
    tcase_add_test(tc_core, test_bitarrayreader_readInteger);
    tcase_add_test(tc_core, test_bitarray_concat);
    tcase_add_test(tc_core, test_bitarray_toBuffer);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *buffer = buffer_suite();
    Suite *queue = priorityqueue_suite();
    Suite *bitarray = bitarray_suite();
    Suite *ringbuffer = ringbuffer_suite();
    SRunner *sr;

    sr = srunner_create(buffer);
    srunner_add_suite(sr, queue);
    srunner_add_suite(sr, bitarray);
    srunner_add_suite(sr, ringbuffer);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
