#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/huffman.h"

START_TEST(test_init_hufftree)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 7, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 3, 'c');
    HuffNode *nodeA = new_huffnode(nodeB, nodeC, 10, '\0');

    ck_assert_ptr_eq(nodeA->left, nodeB);
    ck_assert_ptr_eq(nodeA->right, nodeC);
    ck_assert_ptr_eq(nodeB->left, NULL);
    ck_assert_ptr_eq(nodeB->right, NULL);
    ck_assert_ptr_eq(nodeC->left, NULL);
    ck_assert_ptr_eq(nodeC->right, NULL);
    ck_assert_int_eq(nodeA->key, 10);
    ck_assert_int_eq(nodeB->key, 7);
    ck_assert_int_eq(nodeC->key, 3);
    ck_assert_int_eq(nodeA->value, '\0');
    ck_assert_int_eq(nodeB->value, 'b');
    ck_assert_int_eq(nodeC->value, 'c');
    delete_hufftree(nodeA);
}
END_TEST

START_TEST(test_huffnode_createParent)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 7, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 3, 'c');
    HuffNode *nodeA = huffnode_createParent(nodeB, nodeC);

    ck_assert_ptr_eq(nodeA->left, nodeB);
    ck_assert_ptr_eq(nodeA->right, nodeC);
    ck_assert_ptr_eq(nodeB->left, NULL);
    ck_assert_ptr_eq(nodeB->right, NULL);
    ck_assert_ptr_eq(nodeC->left, NULL);
    ck_assert_ptr_eq(nodeC->right, NULL);
    ck_assert_int_eq(nodeA->key, 10);
    ck_assert_int_eq(nodeB->key, 7);
    ck_assert_int_eq(nodeC->key, 3);
    ck_assert_int_eq(nodeA->value, '\0');
    ck_assert_int_eq(nodeB->value, 'b');
    ck_assert_int_eq(nodeC->value, 'c');
    delete_hufftree(nodeA);
}
END_TEST

START_TEST(test_huffnode_compare)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 7, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 3, 'c');

    ck_assert_int_gt(huffnode_compare(nodeB, nodeC), 0);
    ck_assert_int_lt(huffnode_compare(nodeC, nodeB), 0);
    delete_hufftree(nodeB);
    delete_hufftree(nodeC);
}
END_TEST

START_TEST(test_huffnode_isLeaf)
{
    HuffNode *nodeB = new_huffnode(NULL, NULL, 7, 'b');
    HuffNode *nodeC = new_huffnode(NULL, NULL, 3, 'c');
    HuffNode *nodeA = huffnode_createParent(nodeB, nodeC);

    ck_assert_int_eq(huffnode_isLeaf(nodeA), 0);
    ck_assert_int_ne(huffnode_isLeaf(nodeB), 0);
    delete_hufftree(nodeA);
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

int main(void)
{
    int number_failed;
    Suite *hufftree_s;
    SRunner *sr;

    hufftree_s = hufftree_suite();
    sr = srunner_create(hufftree_s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
