#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/huffman.h"
#include "../src/codedtree.h"

START_TEST(test_init_hufftree)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 3, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 7, 'c');
    HuffNode *nodeA = new_huffnode(nodeB, nodeC, 10, '\0');

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
    delete_hufftree(nodeA);
}
END_TEST

START_TEST(test_huffnode_createParent)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 3, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 7, 'c');
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
    delete_hufftree(nodeA);
}
END_TEST

START_TEST(test_huffnode_compare)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 3, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 7, 'c');

    ck_assert_int_gt(huffnode_compare(nodeB, nodeC), 0);
    ck_assert_int_lt(huffnode_compare(nodeC, nodeB), 0);
    delete_hufftree(nodeB);
    delete_hufftree(nodeC);
}
END_TEST

START_TEST(test_huffnode_isLeaf)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 3, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 7, 'c');
    HuffNode *nodeA = huffnode_createParent(nodeB, nodeC);

    ck_assert_int_eq(huffnode_isLeaf(nodeA), 0);
    ck_assert_int_ne(huffnode_isLeaf(nodeB), 0);
    delete_hufftree(nodeA);
}
END_TEST

START_TEST(test_huffcode_compare)
{
    HuffCode a = (HuffCode){'a', 0x3, 2};
    HuffCode b = (HuffCode){'b', 0x4, 2};

    ck_assert_int_gt(huffcode_compare(&a, &b), -1);
    ck_assert_int_lt(huffcode_compare(&b, &a), 1);
}
END_TEST

START_TEST(test_serialize_hufftree)
{
    HuffNode *b = new_huffnode(NULL, NULL, 2, 'b');
    HuffNode *c = new_huffnode(NULL, NULL, 1, 'c');
    HuffNode *a = new_huffnode(NULL, NULL, 3, 'a');
    HuffNode *bc = huffnode_createParent(b, c);
    HuffNode *abc = huffnode_createParent(a, bc);

    BitArray *ba = serialize_hufftree(abc);
    ck_assert_int_eq(ba->data->data[0], 1);
    ck_assert_int_eq(ba->data->data[1], 2);
    ck_assert_int_eq(ba->data->data[3], 2);
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

Suite *codedtree_suite(void)
{
    Suite *s;
    TCase *tc_core;
    s = suite_create("CodedTree");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_huffcode_compare);
    tcase_add_test(tc_core, test_serialize_hufftree);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *hufftree_s;
    Suite *codedtree_s;
    SRunner *sr;

    hufftree_s = hufftree_suite();
    codedtree_s = codedtree_suite();
    sr = srunner_create(hufftree_s);
    srunner_add_suite(sr, codedtree_s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
