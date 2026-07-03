/* A program for testing the correct operation of 2-3 trees */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <time.h>
// #include <sys/time.h>

#include <math.h>

#include <check.h>

#include "src/tree_2_3/tree_2_3.h"
#include "src/log/log.h"


#define SIZE_ARR(arr)   (sizeof(arr)/sizeof(*arr))


static Tree_2_3 _tree;  /* global object for test cases */

struct memory_counter
{
    unsigned free;
    unsigned alloc;
} *g_memory_counter;



/* ---------- double functions --------------------------------------------- */

static int cmp_double(TreeKey a, TreeKey b)
{
    if (fabs(*(double*)a - *(double*)b) <= 1e-15)
        return 0;

    if (*(double*)a > *(double*)b)
        return 1;

    return -1;
}


static TreeKey copy_double(TreeKey key)
{
    TreeKey tmp = malloc(sizeof(double));
    

    if (tmp == NULL)
        log_error("Can't allocate memory for key!");
    else
    {
        memcpy((void*)tmp, key, sizeof(double));
        if (g_memory_counter)   g_memory_counter->alloc++;
    }
    
    return tmp;
}

static void free_double(TreeKey key)
{
    if (key == NULL)
        log_error("Try free NULL key!");
    else
    {
        free((void*)key);
        if (g_memory_counter)   g_memory_counter->free++;
    }
}


static void print_double(TreeKey value)
{
    printf("%.9lf", *(double*)value);
}


/* ----------- string functions -------------------------------------------- */

/*
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
*/

/* ---------- auxiliary functions ------------------------------------------ */

#define MAKE_TREE(key_type)         tree_create(cmp_##key_type, copy_##key_type, free_##key_type)
#define MAKE_TREE_PLAIN(key_type)   tree_create(cmp_##key_type, NULL, NULL)


static void setup(void)
{
    log_trace("%s", __func__);

    _tree = MAKE_TREE_PLAIN(double);
}


static void teardown(void)
{
    log_trace("%s", __func__);

    tree_destroy(_tree);
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


START_TEST(test_destroy_full_tree)
{
    double vals[] = { 1, 2, 3, 4 };
    int len_vals = (int)SIZE_ARR(vals);

    Tree_2_3 tree = MAKE_TREE(double);
    g_memory_counter = &(struct memory_counter){0};


    for (int i = 0; i < len_vals; i++)
    {
        ck_assert(tree_insert_key(tree, &vals[i]));
    }

    tree_destroy(tree);

    ck_assert_int_eq(g_memory_counter->free, len_vals);
    ck_assert_int_eq(g_memory_counter->free, g_memory_counter->alloc);

    g_memory_counter = NULL;
}
END_TEST


START_TEST(test_make_empty_tree)
{
    double vals[] = { 1, 2, 3, 4 };
    int len_vals = (int)SIZE_ARR(vals);

    Tree_2_3 tree = MAKE_TREE(double);
    g_memory_counter = &(struct memory_counter){0};


    for (int i = 0; i < len_vals; i++)
    {
        ck_assert(tree_insert_key(tree, &vals[i]));
    }

    tree_make_empty(tree);

    ck_assert(tree_is_empty(tree));
    ck_assert_int_eq(tree_height(tree), 0);
    ck_assert_int_eq(tree_count_elements(tree), 0);
    ck_assert_int_eq(g_memory_counter->free, len_vals);
    ck_assert_int_eq(g_memory_counter->free, g_memory_counter->alloc);

    tree_destroy(tree);
    g_memory_counter = NULL;
}
END_TEST


/* ========== INSERT ======================================================= */

START_TEST(test_insert_one_element)
{
    const double key = 10.0;


    ck_assert(tree_insert_key(_tree, &key));

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

    Node_2_3 node = tree_search_key(_tree, &key);
    ck_assert_ptr_nonnull(node);
    ck_assert_double_eq(*(const double *)node_get_key(node), key);
}
END_TEST


START_TEST(test_insert_null)
{
    const double key = 10.0;


    ck_assert(!tree_insert_key(_tree, NULL)); // try insert NULL in empty tree

    /* check that the tree remains empty */
    ck_assert(tree_is_empty(_tree));
    ck_assert_int_eq(tree_height(_tree), 0);
    ck_assert_int_eq(tree_count_elements(_tree), 0);
    ck_assert_ptr_null(tree_get_root(_tree));

    ck_assert(tree_insert_key(_tree, &key));
    ck_assert(!tree_insert_key(_tree, NULL)); // try insert NULL in not empty tree

    /* we check that there is one element left in the tree */
    ck_assert(!tree_is_empty(_tree));
    ck_assert_int_eq(tree_height(_tree), 1);
    ck_assert_int_eq(tree_count_elements(_tree), 1);
    ck_assert_ptr_nonnull(tree_get_root(_tree));

    /* we check that the only element matches the added one */
    TreeKey min = tree_get_min(_tree);
    TreeKey max = tree_get_max(_tree);
    ck_assert_ptr_nonnull(min);
    ck_assert_ptr_nonnull(max);
    ck_assert_double_eq(*(const double *)min, key);
    ck_assert_double_eq(*(const double *)max, key);
}
END_TEST


START_TEST(test_insert_many_ascending)
{
    const double vals[] = { 1, 2, 3, 4, 5, 6, 7 };
    const size_t len_vals = SIZE_ARR(vals);


    for (size_t i = 0; i < len_vals; i++)
    {
        ck_assert(tree_insert_key(_tree, &vals[i]));
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
        ck_assert(tree_insert_key(_tree, &vals[i]));
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


    ck_assert(tree_insert_key(_tree, &key));
    ck_assert(!tree_insert_key(_tree, &key));

    ck_assert_int_eq(tree_height(_tree), 1);
    ck_assert_int_eq(tree_count_elements(_tree), 1);
}
END_TEST


/* ========== REMOVE ======================================================= */

START_TEST(test_remove_element_in_empty_tree)
{    
    const double key = 10;


    ck_assert(!tree_remove_key(_tree, &key));
    ck_assert_ptr_null(tree_get_root(_tree));
    ck_assert_int_eq(tree_height(_tree), 0);
    ck_assert_int_eq(tree_count_elements(_tree), 0);
}
END_TEST


START_TEST(test_remove_null)
{
    const double key = 10.0;


    ck_assert(!tree_remove_key(_tree, NULL)); // try remove NULL from empty tree

    ck_assert(tree_insert_key(_tree, &key));
    ck_assert(!tree_remove_key(_tree, NULL)); // try remove NULL from not empty tree
    ck_assert_int_eq(tree_height(_tree), 1);
    ck_assert_int_eq(tree_count_elements(_tree), 1);
}
END_TEST


START_TEST(test_remove_one_element)
{
    const double key = 10.0;


    ck_assert(tree_insert_key(_tree, &key));

    //Node_2_3 finded = search_key(_tree, &key);
    //ck_assert_ptr_nonnull(finded);
    //ck_assert_double_eq(*(const double *)node_get_key(finded), 10);
    ck_assert(tree_remove_key(_tree, &key));

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
    

    for (int i = 0; i < len_vals; i++)
    {
        ck_assert(tree_insert_key(_tree, &vals[i]));
    }

    ck_assert(!tree_remove_key(_tree, &missing));
    ck_assert_int_eq(tree_count_elements(_tree), len_vals);
}
END_TEST


START_TEST(test_remove_until_empty)
{
    const double vals[] = { 10.0, 20.0, 30.0, 40.0, 50.0 };
    const int len_vals = (int)SIZE_ARR(vals);


    for (int i = 0; i < len_vals; i++)
    {
        ck_assert(tree_insert_key(_tree, &vals[i]));
    }

    for (int i = 0; i < len_vals; i++)
    {
        ck_assert(tree_remove_key(_tree, &vals[i]));
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
    

    for (int i = 0; i < len_vals; i++)
    {
        ck_assert(tree_insert_key(_tree, &vals[i]));
    }

    ck_assert(tree_remove_key(_tree, &vals[element]));
    ck_assert_int_eq(tree_count_elements(_tree), len_vals-1);
}
END_TEST


/* ========== SEARCH ======================================================= */

START_TEST(test_search_in_empty_tree)
{
    const double key = 1;


    ck_assert_ptr_null(tree_search_key(_tree, &key));
    ck_assert_ptr_null(tree_get_min(_tree));
    ck_assert_ptr_null(tree_get_max(_tree));
}
END_TEST


START_TEST(test_search_missing_key)
{
    const double missing = 15.0;
    const double vals[] = { 10.0, 20.0, 30.0 };


    for (size_t i = 0; i < 3; i++) {
        ck_assert(tree_insert_key(_tree, &vals[i]));
    }

    ck_assert_ptr_null(tree_search_key(_tree, &missing));
}
END_TEST


/* ========== COPY_FUNC ==================================================== */

START_TEST(test_no_copy_key_dependent_from_source)
{
    double query = 10.0;
    struct _key { double v; };
    struct _node{ struct _key key; const char* name; } val={.key.v=query, .name="10.0"};

    

    ck_assert(tree_insert_key(_tree, &val));
    val.name = "99.0";

    Node_2_3 node = tree_search_key(_tree, &query);
    ck_assert_ptr_nonnull(node);

    const char *name = ((struct _node*)node_get_key(node))->name;
    ck_assert_str_eq(name, "99.0");
}
END_TEST


START_TEST(test_copy_key_independent_from_source)
{
    double key = 10;
    double query = 10;

    Tree_2_3 tree = MAKE_TREE(double);
    

    ck_assert(tree_insert_key(tree, &key));
    key = 99;

    Node_2_3 node = tree_search_key(tree, &query);
    ck_assert_ptr_nonnull(node);
    ck_assert_double_eq(*(const double*)node_get_key(node), query);

    tree_destroy(tree);
}
END_TEST


/* ========== HEIGHT ======================================================= */

START_TEST(test_height_within_theoretical_bounds)
{
    const int count_vals = 10000;
    int count_cickle = 0;

    Tree_2_3 tree = MAKE_TREE(double);
    

    while (tree_count_elements(tree) < count_vals)
    {
        double key = drand48(); //(double)rand();

        if (!tree_insert_key(tree, &key))
        {
            log_debug("key: %.12lf, count_cickle: %d", key,  count_cickle);
        }

        count_cickle++;

        ck_assert_msg(
            count_cickle < count_vals * 2, 
            "The limit of iterations for filling the tree has been exceeded"
        );
    }

    int height = tree_height(tree);
    double height_max = log2(count_vals) + 1;
    double height_min = log(count_vals) / log(3.0) + 1;

    log_debug("[height] min: %.2lf, max: %.2lf, nominal: %d", height_min, height_max, height);
    ck_assert_double_ge(height, height_min); // 
    ck_assert_double_le(height, height_max);

    tree_destroy(tree);
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

    TCase* tc_destroy_empty = tcase_create("Destroy empty tree");
    tcase_add_test(tc_destroy_empty, test_destroy_empty_tree);
    suite_add_tcase(s, tc_destroy_empty);

    TCase* tc_destroy_full = tcase_create("Destroy full tree");
    tcase_add_test(tc_destroy_full, test_destroy_full_tree);
    suite_add_tcase(s, tc_destroy_full);

    TCase* tc_make_empty = tcase_create("Make empty tree");
    tcase_add_test(tc_make_empty, test_make_empty_tree);
    suite_add_tcase(s, tc_make_empty);

    return s;
}


static Suite* make_suite_insert(void)
{
    Suite* s = suite_create("Insert");

    TCase* tc_insert_null = tcase_create("Insert NULL");
    tcase_add_checked_fixture(tc_insert_null, setup, teardown);
    tcase_add_test(tc_insert_null, test_insert_null);
    suite_add_tcase(s, tc_insert_null);

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

    TCase* tc_remove_null = tcase_create("Remove NULL element");
    tcase_add_checked_fixture(tc_remove_null, setup, teardown);
    tcase_add_test(tc_remove_null, test_remove_null);
    suite_add_tcase(s, tc_remove_null);

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


static Suite* make_suite_copy(void) {
    Suite* s = suite_create("Copy");
    
    TCase* tc_copy_func = tcase_create("Independence TreeKey with user copy function");
    tcase_add_test(tc_copy_func, test_copy_key_independent_from_source);
    suite_add_tcase(s, tc_copy_func);

    TCase* tc_no_copy_func = tcase_create("Dependence TreeKey with no user copy function");
    tcase_add_checked_fixture(tc_no_copy_func, setup, teardown);
    tcase_add_test(tc_no_copy_func, test_no_copy_key_dependent_from_source);
    suite_add_tcase(s, tc_no_copy_func);

    return s;
}


static Suite* make_suite_height(void) {
    Suite* s = suite_create("Height");
    
    TCase* tc_height_bounds = tcase_create("Height within acceptable limits");
    tcase_add_test(tc_height_bounds, test_height_within_theoretical_bounds);
    tcase_set_timeout(tc_height_bounds, 60.0);
    suite_add_tcase(s, tc_height_bounds);

    return s;
}


/* ---------- test --------------------------------------------------------- */

int main(void)
{
    log_set_level(LOG_LEVEL);
    //log_set_quiet(true);
    //srand(time(NULL));
    
    Suite* suite_create_tree = make_suite_create();
    Suite* suite_destroy_tree = make_suite_destroy();

    Suite* suite_insert_key = make_suite_insert();
    Suite* suite_remove_key = make_suite_remove();
    Suite* suite_search_key = make_suite_search();

    Suite* suite_copy_key = make_suite_copy();

    Suite* suite_height_tree = make_suite_height();

    SRunner* sr = srunner_create(suite_create_tree);
    srunner_add_suite(sr, suite_destroy_tree);
    srunner_add_suite(sr, suite_insert_key);
    srunner_add_suite(sr, suite_remove_key);
    srunner_add_suite(sr, suite_search_key);
    srunner_add_suite(sr, suite_copy_key);
    srunner_add_suite(sr, suite_height_tree);


    // srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_VERBOSE);
    //srunner_run(sr, "Height", NULL, CK_VERBOSE);
    int nf = srunner_ntests_failed(sr);

    srunner_free(sr);


    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

    //struct timespec start, end;

    //clock_gettime(CLOCK_REALTIME, &start);
	//clock_gettime(CLOCK_REALTIME, &end);

	//putchar('\n');
	//fprintf(stderr, "Time to work programm %f(clock_gettime)\n", (end.tv_sec - start.tv_sec) + 1e-9*(end.tv_nsec - start.tv_nsec));

    //return 0;
}
