#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/buffer.h"

START_TEST(test_buffer_init) {
    Buffer *buf = new_buffer();
    ck_assert_ptr_ne(buf, NULL);
    ck_assert_ptr_ne(buf->data, NULL);
    ck_assert_int_eq(buf->len, 0);
}
END_TEST

START_TEST(test_buffer_stores_string) {
    Buffer *buf = new_buffer();
    char *testStr = "foo";
    size_t len = strlen(testStr) + 1;
    buffer_append(buf, testStr, len);
    ck_assert_str_eq(buf->data, testStr);
}
END_TEST

START_TEST(test_buffer_append) {
    Buffer *buf = new_buffer();
    char *testStr = "foo";
    buffer_append(buf, testStr, 3); // leaving out null pointer
    buffer_append(buf, testStr, 4);
    ck_assert_str_eq(buf->data, "foofoo");
    ck_assert_int_eq(buf->len, 7);
}
END_TEST

START_TEST(test_buffer_expands) {
    Buffer *buf = new_buffer();
    char testBuf[BUFSIZE * 2];
    buffer_append(buf, testBuf, BUFSIZE * 2); // leaving out null pointer
    ck_assert_int_gt(buf->len, BUFSIZE);
    ck_assert_int_gt(buf->len, BUFSIZE);
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
    tcase_add_test(tc_core, test_buffer_expands);
    suite_add_tcase(s, tc_core);
    
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = buffer_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
