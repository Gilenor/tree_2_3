/* A program for testing the correct operation of 2-3 trees */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include <math.h>

#include <check.h>

#include "src/tree_2_3/tree_2_3.h"
#include "src/log/log.h"


#define SIZE_ARR(arr)   (sizeof(arr)/sizeof(*arr))


static Tree_2_3 _tree;  /* global object for test cases */



/* ---------- strting functions -------------------------------------------- */


static int cmp_double(TreeKey r, TreeKey l)
{
    if (fabs(*(double*)r - *(double*)l) <= 1e-3)
        return 0;

    if (*(double*)r > *(double*)l)
        return 1;

    return -1;
}


static TreeKey copy_double(TreeKey key)
{
    TreeKey tmp = malloc(sizeof(double));

    *(double*)tmp = *(double*)key;

    return tmp;
}


static void print_double(TreeKey value)
{
    printf("%lf", *(double*)value);
}


/* ----------- double functions -------------------------------------------- */


static TreeKey copy_string(TreeKey key)
{
    size_t len = (strlen((char*)key) + 1);
    char *tmp = malloc(sizeof(char) * len);

    strcpy(tmp, (char*)key);

    return (TreeKey)tmp;
}


static int cmp_string(TreeKey r, TreeKey l)
{
    return strcmp((char*)r, (char*)l);
}


static void print_string(TreeKey value)
{
    printf("%s", (char*)value);
}


/* ---------- auxiliary functions ------------------------------------------ */

#define MAKE_TREE(key_type)         tree_create(cmp_##key_type, copy_##key_type, free_##key_type)
#define MAKE_TREE_PLAIN(key_type)   tree_create(cmp_##key_type, NULL, NULL)


static void setup(void)
{
    //static unsigned called = 0;

    //log_debug("Create tree for test case. Times called: %u", called+1);

    _tree = MAKE_TREE_PLAIN(double);
    //called++;
}

static void teardown(void)
{
    //static unsigned called = 0;

    //log_debug("Destroy tree for test case. Times called: %u", called+1);

    tree_destroy(_tree);
    //called++;
}


/* ---------- test cases --------------------------------------------------- */

/* ========== CREATE ======================================================= */

START_TEST(test_create_empty_tree)
{
    Tree_2_3 tree = MAKE_TREE_PLAIN(double);


    ck_assert_ptr_nonnull(tree);
    ck_assert(tree_is_empty(tree));
    ck_assert_int_eq(tree_height(tree), 0);
    ck_assert_int_eq(tree_count_elements(tree), 0);

    ck_assert_ptr_null(tree_get_root(tree));
    ck_assert_ptr_null(tree_get_min(tree));
    ck_assert_ptr_null(tree_get_max(tree));

    tree_destroy(tree);
}
END_TEST


/* ========== DESTROY ====================================================== */

START_TEST(test_destroy_empty_tree)
{
    Tree_2_3 tree = MAKE_TREE_PLAIN(double);

    tree_destroy(tree);
}
END_TEST


/* ========== INSERT ======================================================= */

START_TEST(test_insert_one_element)
{
    const double key = 10.0;


    ck_assert(insert_key(_tree, &key));

    ck_assert(!tree_is_empty(_tree));
    ck_assert_int_eq(tree_height(_tree), 1);
    ck_assert_int_eq(tree_count_elements(_tree), 1);
    ck_assert_ptr_nonnull(tree_get_root(_tree));

    TreeKey min = tree_get_min(_tree);
    TreeKey max = tree_get_max(_tree);
    ck_assert_ptr_nonnull(min);
    ck_assert_ptr_nonnull(max);
    ck_assert_double_eq(*(const double *)min, key);
    ck_assert_double_eq(*(const double *)max, key);

    Node_2_3 node = search_key(_tree, &key);
    ck_assert_ptr_nonnull(node);
    ck_assert_double_eq(*(const double *)node_get_key(node), key);
}
END_TEST


START_TEST(test_insert_many_ascending)
{
    const double vals[] = { 1, 2, 3, 4, 5, 6, 7 };
    const size_t len_vals = SIZE_ARR(vals);


    for (size_t i = 0; i < len_vals; i++)
    {
        ck_assert(insert_key(_tree, &vals[i]));
    }

    TreeKey min = tree_get_min(_tree);
    TreeKey max = tree_get_max(_tree);
    ck_assert_ptr_nonnull(min);
    ck_assert_ptr_nonnull(max);

    ck_assert_int_eq(tree_count_elements(_tree), len_vals);
    ck_assert_double_eq(*(const double *)min, vals[0]);
    ck_assert_double_eq(*(const double *)max, vals[len_vals-1]);
}
END_TEST


START_TEST(test_insert_many_descending)
{
    const double vals[] = { 7, 6, 5, 4, 3, 2, 1 };
    const int len_vals = (int)SIZE_ARR(vals);


    for (int i = 0; i < len_vals; i++)
    {
        ck_assert(insert_key(_tree, &vals[i]));
    }

    TreeKey min = tree_get_min(_tree);
    TreeKey max = tree_get_max(_tree);
    ck_assert_ptr_nonnull(min);
    ck_assert_ptr_nonnull(max);

    ck_assert_int_eq(tree_count_elements(_tree), len_vals);
    ck_assert_double_eq(*(const double *)min, vals[len_vals-1]);
    ck_assert_double_eq(*(const double *)max, vals[0]);
}
END_TEST


START_TEST(test_insert_duplicate)
{
    const double key = 42;


    ck_assert(insert_key(_tree, &key));
    ck_assert(!insert_key(_tree, &key));

    ck_assert_int_eq(tree_height(_tree), 1);
    ck_assert_int_eq(tree_count_elements(_tree), 1);
}
END_TEST


/* ========== REMOVE ======================================================= */

START_TEST(test_remove_element_in_empty_tree)
{    
    const double key = 10;


    ck_assert(!remove_key(_tree, &key));
    ck_assert_ptr_null(tree_get_root(_tree));
    ck_assert_int_eq(tree_height(_tree), 0);
    ck_assert_int_eq(tree_count_elements(_tree), 0);
}
END_TEST


START_TEST(test_remove_one_element)
{
    const double x = 10.0;


    insert_key(_tree, &x);

    Node_2_3 finded = search_key(_tree, &x);
    ck_assert_ptr_nonnull(finded);
    ck_assert_double_eq(*(const double *)node_get_key(finded), 10);
    ck_assert(remove_key(_tree, node_get_key(finded)));

    ck_assert_ptr_null(tree_get_root(_tree));
    ck_assert_int_eq(tree_height(_tree), 0);
    ck_assert_int_eq(tree_count_elements(_tree), 0);
}
END_TEST


START_TEST(test_remove_missing_element)
{
    const double missing = 99.0;
    const double vals[] = { 10.0, 20.0, 30.0 };
    const int len_vals = (int)SIZE_ARR(vals);
    

    for (size_t i = 0; i < len_vals; i++)
    {
        ck_assert(insert_key(_tree, &vals[i]));
    }

    ck_assert(!remove_key(_tree, &missing));
    ck_assert_int_eq(tree_count_elements(_tree), len_vals);
}
END_TEST


START_TEST(test_remove_until_empty)
{
    const double vals[] = { 10.0, 20.0, 30.0, 40.0, 50.0 };
    const int len_vals = (int)SIZE_ARR(vals);


    for (size_t i = 0; i < len_vals; i++)
    {
        ck_assert(insert_key(_tree, &vals[i]));
    }

    for (size_t i = 0; i < len_vals; i++)
    {
        ck_assert(remove_key(_tree, &vals[i]));
    }

    ck_assert(tree_is_empty(_tree));

    ck_assert_int_eq(tree_height(_tree), 0);
    ck_assert_int_eq(tree_count_elements(_tree), 0);

    ck_assert_ptr_null(tree_get_root(_tree));
    ck_assert_ptr_null(tree_get_min(_tree));
    ck_assert_ptr_null(tree_get_max(_tree));
}
END_TEST


START_TEST(test_remove_random_element)
{
    const double vals[] = { 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0 };
    const int len_vals = (int)SIZE_ARR(vals);
    const int element = rand() % len_vals;

    log_debug("Random element: %lf, index: %d", vals[element], element);
    

    for (size_t i = 0; i < len_vals; i++)
    {
        ck_assert(insert_key(_tree, &vals[i]));
    }

    ck_assert(remove_key(_tree, &vals[element]));
    ck_assert_int_eq(tree_count_elements(_tree), len_vals-1);
}
END_TEST


/* ========== SEARCH ======================================================= */

START_TEST(test_search_in_empty_tree)
{
    const double key = 1;


    ck_assert_ptr_null(search_key(_tree, &key));
    ck_assert_ptr_null(tree_get_min(_tree));
    ck_assert_ptr_null(tree_get_max(_tree));
}
END_TEST


START_TEST(test_search_missing_key)
{
    const double missing = 15.0;
    const double vals[] = { 10.0, 20.0, 30.0 };


    for (size_t i = 0; i < 3; i++) {
        ck_assert(insert_key(_tree, &vals[i]));
    }

    ck_assert_ptr_null(search_key(_tree, &missing));
}
END_TEST


/* ---------- suites ------------------------------------------------------- */

static Suite* make_suite_create(void) {
    Suite* s = suite_create("Create");
    
    TCase* tc_create = tcase_create("Create tree");
    tcase_add_test(tc_create, test_create_empty_tree);
    suite_add_tcase(s, tc_create);

    return s;
}


static Suite* make_suite_destroy(void) {
    Suite* s = suite_create("Destroy");

    TCase* tc_destroy = tcase_create("Destroy tree");
    tcase_add_test(tc_destroy, test_destroy_empty_tree);
    suite_add_tcase(s, tc_destroy);

    return s;
}


static Suite* make_suite_insert(void)
{
    Suite* s = suite_create("Insert");

    TCase* tc_insert_one = tcase_create("Insert one element");
    tcase_add_checked_fixture(tc_insert_one, setup, teardown);
    tcase_add_test(tc_insert_one, test_insert_one_element);
    suite_add_tcase(s, tc_insert_one);

    TCase* tc_insert_many_asc = tcase_create("Insert many elements ascending");
    tcase_add_checked_fixture(tc_insert_many_asc, setup, teardown);
    tcase_add_test(tc_insert_many_asc, test_insert_many_ascending);
    suite_add_tcase(s, tc_insert_many_asc);

    TCase* tc_insert_many_desc = tcase_create("Insert many elements descending");
    tcase_add_checked_fixture(tc_insert_many_desc, setup, teardown);
    tcase_add_test(tc_insert_many_desc, test_insert_many_descending);
    suite_add_tcase(s, tc_insert_many_desc);

    TCase* tc_insert_duplicate = tcase_create("Insert duplicated element");
    tcase_add_checked_fixture(tc_insert_duplicate, setup, teardown);
    tcase_add_test(tc_insert_duplicate, test_insert_duplicate);
    suite_add_tcase(s, tc_insert_duplicate);

    return s;
}


static Suite* make_suite_remove(void)
{
    Suite* s = suite_create("Remove");

    TCase* tc_remove_empty = tcase_create("Remove element in empty tree");
    tcase_add_checked_fixture(tc_remove_empty, setup, teardown);
    tcase_add_test(tc_remove_empty, test_remove_element_in_empty_tree);
    suite_add_tcase(s, tc_remove_empty);

    TCase* tc_remove_one = tcase_create("Remove one element");
    tcase_add_checked_fixture(tc_remove_one, setup, teardown);
    tcase_add_test(tc_remove_one, test_remove_one_element);
    suite_add_tcase(s, tc_remove_one);

    TCase* tc_remove_missing = tcase_create("Remove missing element");
    tcase_add_checked_fixture(tc_remove_missing, setup, teardown);
    tcase_add_test(tc_remove_missing, test_remove_missing_element);
    suite_add_tcase(s, tc_remove_missing);

    TCase* tc_remove_until_emtpy = tcase_create("Remove elements until tree not empty");
    tcase_add_checked_fixture(tc_remove_until_emtpy, setup, teardown);
    tcase_add_test(tc_remove_until_emtpy, test_remove_until_empty);
    suite_add_tcase(s, tc_remove_until_emtpy);

    TCase* tc_remove_random_element = tcase_create("Remove random element");
    tcase_add_checked_fixture(tc_remove_random_element, setup, teardown);
    tcase_add_test(tc_remove_random_element, test_remove_random_element);
    suite_add_tcase(s, tc_remove_random_element);
    

    return s;
}


static Suite* make_suite_search(void)
{
    Suite* s = suite_create("Search");

    TCase* tc_search_missing = tcase_create("Search missing element");
    tcase_add_checked_fixture(tc_search_missing, setup, teardown);
    tcase_add_test(tc_search_missing, test_search_missing_key);
    suite_add_tcase(s, tc_search_missing);

    TCase* tc_search_in_empty = tcase_create("Search element in empty tree");
    tcase_add_checked_fixture(tc_search_in_empty, setup, teardown);
    tcase_add_test(tc_search_in_empty, test_search_in_empty_tree);
    suite_add_tcase(s, tc_search_in_empty);

    return s;
}


/* ---------- test --------------------------------------------------------- */

int main(void)
{
    log_set_level(LOG_LEVEL);
    //log_set_level(LOG_TRACE);
    srand(time(NULL));
    
    Suite* suite_create_tree = make_suite_create();
    Suite* suite_destroy_tree = make_suite_destroy();

    Suite* suite_insert_key = make_suite_insert();
    Suite* suite_remove_key = make_suite_remove();
    Suite* suite_search_key = make_suite_search();

    SRunner* sr = srunner_create(suite_create_tree);
    srunner_add_suite(sr, suite_destroy_tree);
    srunner_add_suite(sr, suite_insert_key);
    srunner_add_suite(sr, suite_remove_key);
    srunner_add_suite(sr, suite_search_key);


    // srunner_set_fork_status(sr, CK_NOFORK);
    //srunner_run_all(sr, CK_VERBOSE);
    srunner_run(sr, "Remove", NULL, CK_VERBOSE);
    int nf = srunner_ntests_failed(sr);

    srunner_free(sr);


    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

    //struct timespec start2, end2;

    //clock_gettime(CLOCK_REALTIME, &start2);
	//clock_gettime(CLOCK_REALTIME, &end2);

	//putchar('\n');
	//fprintf(stderr, "Time to work programm %f(clock_gettime)\n", (end2.tv_sec - start2.tv_sec) + 1e-9*(end2.tv_nsec - start2.tv_nsec));

    //return 0;
}
