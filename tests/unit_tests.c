#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/bitarray.h"
#include "../src/buffer.h"
#include "../src/queue.h"

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
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *buffer = buffer_suite();
    Suite *queue = priorityqueue_suite();
    Suite *bitarray = bitarray_suite();
    SRunner *sr;

    sr = srunner_create(buffer);
    srunner_add_suite(sr, queue);
    srunner_add_suite(sr, bitarray);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
