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
    ck_assert_int_eq(result, 4);
}
END_TEST

Suite *lzss_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("LZSS");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, testFindMatch);
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
