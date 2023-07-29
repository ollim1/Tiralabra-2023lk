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


