#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <check.h>

#include "hashmap/hashmap.h"
#include "log/log.h"


#define SIZE_ARR(arr)   (int)(sizeof(arr) / sizeof(*arr))

#define MAKE_HASHMAP(kv_type)           hm_create(cmp_##kv_type, copy_##kv_type, free_##kv_type)
#define MAKE_PLAIN_HASHMAP(kv_type)     hm_create(cmp_##kv_type, NULL, NULL)

#define KEY_FROM_DATE(year, mon, day)   \
(((((uint64_t)(year) << 16) | ((mon) & 0xFFFF)) << 16) | ((day) & 0xFFFF))

#define YEAR(date)      (int)((uint64_t)(date) >> 32)
#define MONTH(date)     (int)(((uint64_t)(date) >> 16) & 0xFFFF)
#define DAY(date)       (int)((uint64_t)(date) & 0xFFFF)


#define INC_MEM_COUNTER(inc_type) \
if (g_memory_counter) \
{ \
    g_memory_counter.inc_type++; \
} \
else \
{ \
    log_debug("g_memory_counter is NULL (%s)", __func__); \
} \


struct memory_counter
{
    unsigned alloc;
    unsigned free;
} g_memory_counter;  /* use for count copy/free function calls */

static HashMap *_hashmap;  /* global object for test cases */



/* ---------- key/value functions ------------------------------------------ */


/* key: время, value: описание события */
typedef uint64_t Key;
typedef const char * Value;

typedef struct pair
{
    Key key;
    Value value;
} Pair;


static Key today(void)
{
    time_t timer = time(NULL);
    struct tm now = *gmtime(&timer);

    Key key = KEY_FROM_DATE(now.tm_year+1900, now.tm_mon+1, now.tm_mday);

    return key;
}


static const char* key_to_str(Key key)
{
    log_trace("%s", __func__);

    static char *monts[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static char buf[128];

    int day = key & 0xFFFF;         key >>= 16;
    int mon = key & 0xFFFF;         key >>= 16;
    int year = key & 0xFFFFFFFF;     

    char *era = year < 0 ? " B.C." : "";
    year = year < 0 ? -year : year;


    sprintf(buf, "%s %2d %d%s", monts[mon], day, year, era);

    return buf;
}


static int cmp_kv(const KeyVal *a, const KeyVal *b)
{
    log_trace("%s", __func__);

    Key time_a = *(Key*)KEY(a);
    Key time_b = *(Key*)KEY(b);


    if (YEAR(time_a) < YEAR(time_b))    return -1;
    if (YEAR(time_a) > YEAR(time_b))    return  1;

    if (MONTH(time_a) < MONTH(time_b))  return -1;
    if (MONTH(time_a) > MONTH(time_b))  return  1;

    return DAY(time_a) - DAY(time_b);
}


static KeyVal copy_kv(const KeyVal *kv)
{
    log_trace("%s", __func__);

    //INC_MEM_COUNTER(alloc);
    g_memory_counter.alloc++;
    log_debug("counter_alloc: %d", g_memory_counter.alloc);

    HmKey key = memcpy(malloc(sizeof(Key)), kv->key, sizeof(Key));
    HmValue value = strdup(kv->value);
    
    return (KeyVal){ .key=key, .value=value };
}


static void free_kv(const KeyVal *key)
{
    log_trace("%s", __func__);

    if (!key)
    {
        log_warn("Try free NULL key!");
        return;
    }

    //INC_MEM_COUNTER(free);
    g_memory_counter.free++;
    log_debug("counter_free: %d", g_memory_counter.free);
    
    free((void*)KEY(key));
    free((void*)VALUE(key));
    //free((void*)key);
}


static void print_kv(const KeyVal *kv)
{
    log_trace("%s", __func__);

    Key key = *(Key*)KEY(kv);
    Value val = (Value)VALUE(kv);

    
    printf("[%s] - %s", key_to_str(key), val);
}


/* ---------- auxiliary functions ------------------------------------------ */

static Pair g_test_data[] = 
{ 
    { KEY_FROM_DATE(-3100, 1, 1), "объединение Верхнего и Нижнего Египта, начало Раннединастического периода."},  
    { KEY_FROM_DATE(-1754, 1, 1), "принятие законов Хаммурапи в Вавилоне." },  
    { KEY_FROM_DATE(-776,  7, 1), "первые Олимпийские игры в Древней Греции." },
    { KEY_FROM_DATE(-509,  5, 1), "установление Римской республики." },
    { KEY_FROM_DATE(-221, 10, 1), "Цинь Шихуанди объединил Китай, основание империи Цинь." },

    { KEY_FROM_DATE(476,   9,  4), "падение Западной Римской империи." },
    { KEY_FROM_DATE(1054,  7, 16), "разделение христианской церкви на Восточную и Западную." },
    { KEY_FROM_DATE(1215,  6, 15), "подписание Великой хартии вольностей." },
    { KEY_FROM_DATE(1492, 10, 12), "высадка Колумба в Америке." },
    { KEY_FROM_DATE(1517, 10, 31), "начало Реформации (тезисы Лютера)." },
    { KEY_FROM_DATE(1776,  7,  4), "принятие Декларации независимости США." },
    { KEY_FROM_DATE(1789,  7, 14), "взятие Бастилии, начало Великой французской революции." },
    { KEY_FROM_DATE(1914,  7, 28), "начало Первой мировой войны." },
    { KEY_FROM_DATE(1945,  8,  6), "атомная бомбардировка Хиросимы." },  
    { KEY_FROM_DATE(1969,  7, 20), "высадка человека на Луну." }
};


static int fill_hashmap_test_data(HashMap *hashmap)
{
    for (int i = 0; i < SIZE_ARR(g_test_data); i++)
    {
        Key *key = &g_test_data[i].key;
        Value val = g_test_data[i].value;

        ck_assert(hm_insert_kv(hashmap, key, (HmValue)val));
    }

    ck_assert_int_eq(hm_length(hashmap), SIZE_ARR(g_test_data));

    return SIZE_ARR(g_test_data);
}


static void setup(void)
{
    log_trace("%s", __func__);

    _hashmap = MAKE_PLAIN_HASHMAP(kv);
    ck_assert_ptr_nonnull(_hashmap);
}


static void teardown(void)
{
    log_trace("%s", __func__);

    hm_destroy(&_hashmap);
    ck_assert_ptr_null(_hashmap);
}


static void setup_data(void)
{
    log_trace("%s", __func__);

    _hashmap = MAKE_HASHMAP(kv);
    g_memory_counter = (struct memory_counter){0};

    ck_assert_ptr_nonnull(_hashmap);

    fill_hashmap_test_data(_hashmap);
}


static void teardown_data(void)
{
    log_trace("%s", __func__);

    hm_destroy(&_hashmap);
    ck_assert_ptr_null(_hashmap);

    g_memory_counter = (struct memory_counter){0};
}


/* ---------- test cases --------------------------------------------------- */


/* ========== CREATE ======================================================= */

START_TEST(test_create_empty_hashmap)
{;
    g_memory_counter = (struct memory_counter){0};
    HashMap *hashmap = MAKE_HASHMAP(kv);


    ck_assert_ptr_nonnull(hashmap);
    ck_assert_int_eq(hm_length(hashmap), 0);

    ck_assert_int_eq(g_memory_counter.alloc, 0);

    hm_destroy(&hashmap);
    g_memory_counter = (struct memory_counter){0};
}
END_TEST


/* ========== DESTROY ====================================================== */


START_TEST(test_destroy_empty_hashmap)
{
    g_memory_counter = (struct memory_counter){0};
    HashMap *hashmap = MAKE_HASHMAP(kv);


    hm_destroy(&hashmap);
    ck_assert_ptr_null(hashmap);

    ck_assert_int_eq(g_memory_counter.free, 0);

    g_memory_counter = (struct memory_counter){0};
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_destroy_hashmap_no_copy_free)
{
    g_memory_counter = (struct memory_counter){0};
    

    fill_hashmap_test_data(_hashmap);

    ck_assert_int_eq(g_memory_counter.alloc, 0);
    ck_assert_int_eq(g_memory_counter.free, 0);

    g_memory_counter = (struct memory_counter){0};
}
END_TEST


/* using fixtures - setup_data/NULL callbacks */
START_TEST(test_destroy_hashmap_with_copy_free)
{
    int len_vals = SIZE_ARR(g_test_data);


    hm_destroy(&_hashmap);
    ck_assert_ptr_null(_hashmap);

    ck_assert_int_eq(g_memory_counter.alloc, len_vals);
    ck_assert_int_eq(g_memory_counter.free, len_vals);

    g_memory_counter = (struct memory_counter){0};
}
END_TEST


/* ========== CLEAR ======================================================== */


/* using fixtures - setup_data/teardown_data callbacks */
START_TEST(test_clear_hashmap)
{
    int len_vals = SIZE_ARR(g_test_data);


    hm_clear(_hashmap);
    ck_assert_int_eq(hm_length(_hashmap), 0);

    ck_assert_int_eq(g_memory_counter.alloc, len_vals);
    ck_assert_int_eq(g_memory_counter.free, len_vals);
}
END_TEST


START_TEST(test_clear_and_reuse_hashmap)
{
    int reuse = 2;
    int len_vals = SIZE_ARR(g_test_data);

    HashMap *hashmap = MAKE_HASHMAP(kv);
    g_memory_counter = (struct memory_counter){0};


    for (int j = 0; j < reuse; j++)
    {
        fill_hashmap_test_data(hashmap);

        hm_clear(hashmap);
        ck_assert_int_eq(hm_length(hashmap), 0);

        for (int i = 0; i < len_vals; i++)
        {
            Key key = g_test_data[i].key;
            ck_assert(!hm_is_contain(hashmap, &key));
            ck_assert_ptr_null(hm_get_value(hashmap, &key));
        }
    }

    ck_assert_int_eq(g_memory_counter.alloc, len_vals * reuse);
    ck_assert_int_eq(g_memory_counter.free, len_vals * reuse);

    hm_destroy(&hashmap);
    g_memory_counter = (struct memory_counter){0};
}
END_TEST


/* ========== INSERT ======================================================= */

/* using fixtures - setup/teardown callbacks */
START_TEST(test_insert_null_key)
{
    ck_assert(!hm_insert_kv(_hashmap, NULL, NULL));

    /* check that the hashmap remains empty */
    ck_assert_int_eq(hm_length(_hashmap), 0);
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_insert_null_value)
{
    Key key = today();


    ck_assert(hm_insert_kv(_hashmap, &key, NULL));

    ck_assert(hm_is_contain(_hashmap, &key));
    ck_assert_int_eq(hm_length(_hashmap), 1);

    HmValue hm_val = hm_get_value(_hashmap, &key);
    ck_assert_ptr_null(hm_val);
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_insert_one_key)
{
    Key key = today();
    Value val = "Try test <test_insert_one_key> function";


    ck_assert(hm_insert_kv(_hashmap, &key, (HmValue)val));

    ck_assert_int_eq(hm_length(_hashmap), 1);
    ck_assert(hm_is_contain(_hashmap, &key));

    HmValue hm_val = hm_get_value(_hashmap, &key);
    ck_assert_ptr_nonnull(hm_val);
    ck_assert_str_eq(val, hm_val);
}
END_TEST


/* using fixtures - setup_data/teardown_data callbacks */
START_TEST(test_insert_many_keys)
{
    /* all work takes place in fixtures */
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_insert_duplicate_key)
{
    Key key = today();
    Value val_original = "Try test <test_insert_duplicate_key> function";
    Value val_replaced = "Replaced value";


    ck_assert(hm_insert_kv(_hashmap, &key, (HmValue)val_original));
    ck_assert_int_eq(hm_length(_hashmap), 1);
    ck_assert(hm_is_contain(_hashmap, &key));
    ck_assert_str_eq(hm_get_value(_hashmap, &key), val_original);

    /* inserting a duplicate key is allowed, the value is simply "overwritten" */
    ck_assert(hm_insert_kv(_hashmap, &key, (HmValue)val_replaced));
    ck_assert_int_eq(hm_length(_hashmap), 1);
    ck_assert(hm_is_contain(_hashmap, &key));
    ck_assert_str_eq(hm_get_value(_hashmap, &key), val_replaced);
}
END_TEST


/* ========== REMOVE ======================================================= */

/* using fixtures - setup/teardown callbacks */
START_TEST(test_remove_key_in_empty_hashmap)
{
    Key key = today();

    ck_assert(!hm_remove_key(_hashmap, &key));
    ck_assert_int_eq(hm_length(_hashmap), 0);
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_remove_null_key)
{
    Key key = today();
    Value val = "Try test <test_remove_null_key> function";


    ck_assert(!hm_remove_key(_hashmap, NULL));  // try remove NULL from empty hashmap
    ck_assert_int_eq(hm_length(_hashmap), 0);

    ck_assert(hm_insert_kv(_hashmap, &key, (HmValue)val));
    ck_assert(!hm_remove_key(_hashmap, NULL)); // try remove NULL from not empty tree
    ck_assert_int_eq(hm_length(_hashmap), 1);

    HmValue hm_val = hm_get_value(_hashmap, &key);

    ck_assert(hm_is_contain(_hashmap, &key));
    ck_assert_ptr_nonnull(hm_val);
    ck_assert_str_eq(val, hm_val);
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_remove_one_key)
{
    Key key = today();
    Value val = "Try test <test_remove_one_key> function";


    ck_assert(hm_insert_kv(_hashmap, &key, (HmValue)val));

    Value hm_val = hm_get_value(_hashmap, &key);
    ck_assert_ptr_nonnull(hm_val);
    ck_assert_str_eq(hm_val, val);
    
    ck_assert(hm_remove_key(_hashmap, &key));
    ck_assert(!hm_is_contain(_hashmap, &key));
    ck_assert_int_eq(hm_length(_hashmap), 0);
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_remove_missing_key)
{
    Key missing = KEY_FROM_DATE(0, 0, 0);
    const int hashmap_len = fill_hashmap_test_data(_hashmap);


    ck_assert(!hm_remove_key(_hashmap, &missing));
    ck_assert_int_eq(hm_length(_hashmap), hashmap_len);
}
END_TEST


/* using fixtures - setup_data/teardown_data callbacks */
START_TEST(test_remove_keys_until_empty)
{
    const int hashmap_len = hm_length(_hashmap);


    for (int i = 0; i < hashmap_len; i++)
    {
        Key key = g_test_data[i].key;

        ck_assert(hm_remove_key(_hashmap, (HmKey)&key));
        ck_assert(!hm_is_contain(_hashmap, (HmKey)&key));
    }

    ck_assert_int_eq(hm_length(_hashmap), 0);
    ck_assert_int_eq(g_memory_counter.free, hashmap_len);
}
END_TEST


/* using fixtures - setup_data/teardown_data callbacks */
START_TEST(test_remove_random_key)
{
    const int hashmap_len = hm_length(_hashmap);
    const int element = rand() % hashmap_len;
    Key key = g_test_data[element].key;


    log_debug("Remove random key: %llu, index: %d", key, element);

    ck_assert(hm_remove_key(_hashmap, &key));
    ck_assert(!hm_is_contain(_hashmap, &key));

    ck_assert_int_eq(g_memory_counter.free, 1);
    ck_assert_int_eq(hm_length(_hashmap), hashmap_len-1);
}
END_TEST


/* ========== SEARCH ======================================================= */

/* using fixtures - setup/teardown callbacks */
START_TEST(test_search_in_empty_tree)
{
    Key key = today();

    ck_assert(!hm_is_contain(_hashmap, &key));
}
END_TEST


/* using fixtures - setup/teardown callbacks */
START_TEST(test_search_missing_key)
{
    Key missing = KEY_FROM_DATE(0, 0, 0);
    fill_hashmap_test_data(_hashmap);

    ck_assert(!hm_is_contain(_hashmap, &missing));
}
END_TEST


/* using fixtures - setup_data/teardown_data callbacks */
START_TEST(test_search_random_key)
{
    const int hashmap_len = hm_length(_hashmap);
    const int element = rand() % hashmap_len;

    Key key = g_test_data[element].key;
    Value val = g_test_data[element].value;


    log_debug("Search random key: %llu, index: %d", key, element);

    ck_assert(hm_is_contain(_hashmap, &key));
    ck_assert_str_eq(hm_get_value(_hashmap, &key), val);

}
END_TEST


/* ========== COPY_FUNC ==================================================== */

/* using fixtures - setup/teardown callbacks */
START_TEST(test_no_copy_dependent_from_source)
{
    Key key = today();
    char buf[128] = "Try test <test_no_copy_dependent_from_source> function";
    Value val_origin = buf;

    ck_assert(hm_insert_kv(_hashmap, &key, (HmValue)val_origin));
    strcpy((char*)val_origin, "Change value for dependent memory");

    Value val_finded = hm_get_value(_hashmap, &key);
    ck_assert_ptr_nonnull(val_finded);
    ck_assert_ptr_eq(val_finded, val_origin);
    ck_assert_str_eq(val_finded, val_origin);
}
END_TEST


START_TEST(test_copy_independent_from_source)
{
    Key key = today();
    char buf[128] = "Try test <test_copy_independent_from_source> function";
    Value val_origin = buf;

    HashMap *hashmap = MAKE_HASHMAP(kv);
    g_memory_counter = (struct memory_counter){0};


    ck_assert(hm_insert_kv(hashmap, &key, (HmValue)val_origin));
    
    // different memory areas with the same content are used
    Value val_finded = hm_get_value(hashmap, &key);
    ck_assert_ptr_nonnull(val_finded);
    ck_assert_ptr_ne(val_finded, val_origin);
    ck_assert_str_eq(val_finded, val_origin);

    // overwrite original content
    strcpy((char*)val_origin, "Change value for dependent memory");

    // different memory areas with different contents are used
    val_finded = hm_get_value(hashmap, &key);
    ck_assert_ptr_nonnull(val_finded);
    ck_assert_ptr_ne(val_finded, val_origin);
    ck_assert_str_ne(val_finded, val_origin);

    hm_destroy(&hashmap);
    ck_assert_ptr_null(hashmap);

    ck_assert_int_eq(g_memory_counter.alloc, 1);
    ck_assert_int_eq(g_memory_counter.free, 1);

    g_memory_counter = (struct memory_counter){0};
}
END_TEST


/* ---------- suites ------------------------------------------------------- */

static Suite* make_suite_create(void)
{
    Suite* s = suite_create("Create");
    
    TCase* tc_create_empty = tcase_create("Create hashmap");
    tcase_add_test(tc_create_empty, test_create_empty_hashmap);
    suite_add_tcase(s, tc_create_empty);

    return s;
}


static Suite* make_suite_destroy(void)
{
    Suite* s = suite_create("Destroy");
    
    TCase* tc_destroy = tcase_create("Destroy empty hashmap");
    tcase_add_test(tc_destroy, test_destroy_empty_hashmap);
    suite_add_tcase(s, tc_destroy);

    TCase* tc_destroy_full_no_manage = tcase_create("Destroy hashmap no memory manage");
    tcase_add_checked_fixture(tc_destroy_full_no_manage, setup, teardown);
    tcase_add_test(tc_destroy_full_no_manage, test_destroy_hashmap_no_copy_free);
    suite_add_tcase(s, tc_destroy_full_no_manage);

    TCase* tc_destroy_full_with_manage = tcase_create("Destroy hashmap with memory manage");
    tcase_add_checked_fixture(tc_destroy_full_with_manage, setup_data, NULL);
    tcase_add_test(tc_destroy_full_with_manage, test_destroy_hashmap_with_copy_free);
    suite_add_tcase(s, tc_destroy_full_with_manage);

    return s;
}


static Suite* make_suite_clear(void)
{
    Suite* s = suite_create("Clear");
    
    TCase* tc_clear = tcase_create("Clear hashmap");
    tcase_add_checked_fixture(tc_clear, setup_data, teardown_data);
    tcase_add_test(tc_clear, test_clear_hashmap);
    suite_add_tcase(s, tc_clear);

    TCase* tc_clear_reuse = tcase_create("Clear and reuse hashmap");
    tcase_add_test(tc_clear_reuse, test_clear_and_reuse_hashmap);
    suite_add_tcase(s, tc_clear_reuse);
    
    return s;
}


static Suite* make_suite_insert(void)
{
    Suite* s = suite_create("Insert");

    TCase* tc_insert_null_key = tcase_create("Insert NULL key");
    tcase_add_checked_fixture(tc_insert_null_key, setup, teardown);
    tcase_add_test(tc_insert_null_key, test_insert_null_key);
    suite_add_tcase(s, tc_insert_null_key);

    TCase* tc_insert_null_value = tcase_create("Insert NULL value");
    tcase_add_checked_fixture(tc_insert_null_value, setup, teardown);
    tcase_add_test(tc_insert_null_value, test_insert_null_value);
    suite_add_tcase(s, tc_insert_null_value);

    TCase* tc_insert_one = tcase_create("Insert one key/value");
    tcase_add_checked_fixture(tc_insert_one, setup, teardown);
    tcase_add_test(tc_insert_one, test_insert_one_key);
    suite_add_tcase(s, tc_insert_one);

    TCase* tc_insert_many = tcase_create("Insert many key/value");
    tcase_add_checked_fixture(tc_insert_many, setup_data, teardown_data);
    tcase_add_test(tc_insert_many, test_insert_many_keys);
    suite_add_tcase(s, tc_insert_many);

    TCase* tc_insert_duplicate = tcase_create("Insert duplicated key");
    tcase_add_checked_fixture(tc_insert_duplicate, setup, teardown);
    tcase_add_test(tc_insert_duplicate, test_insert_duplicate_key);
    suite_add_tcase(s, tc_insert_duplicate);

    return s;
}


static Suite* make_suite_remove(void)
{
    Suite* s = suite_create("Remove");

    TCase* tc_remove_empty = tcase_create("Remove key in empty hashmap");
    tcase_add_checked_fixture(tc_remove_empty, setup, teardown);
    tcase_add_test(tc_remove_empty, test_remove_key_in_empty_hashmap);
    suite_add_tcase(s, tc_remove_empty);

    TCase* tc_remove_null = tcase_create("Remove NULL key");
    tcase_add_checked_fixture(tc_remove_null, setup, teardown);
    tcase_add_test(tc_remove_null, test_remove_null_key);
    suite_add_tcase(s, tc_remove_null);

    TCase* tc_remove_one = tcase_create("Remove one key");
    tcase_add_checked_fixture(tc_remove_one, setup, teardown);
    tcase_add_test(tc_remove_one, test_remove_one_key);
    suite_add_tcase(s, tc_remove_one);

    TCase* tc_remove_missing = tcase_create("Remove missing key");
    tcase_add_checked_fixture(tc_remove_missing, setup, teardown);
    tcase_add_test(tc_remove_missing, test_remove_missing_key);
    suite_add_tcase(s, tc_remove_missing);

    TCase* tc_remove_until_emtpy = tcase_create("Remove keys until hashmap not empty");
    tcase_add_checked_fixture(tc_remove_until_emtpy, setup_data, teardown_data);
    tcase_add_test(tc_remove_until_emtpy, test_remove_keys_until_empty);
    suite_add_tcase(s, tc_remove_until_emtpy);

    TCase* tc_remove_random_element = tcase_create("Remove random key");
    tcase_add_checked_fixture(tc_remove_random_element, setup_data, teardown_data);
    tcase_add_test(tc_remove_random_element, test_remove_random_key);
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

    TCase* tc_search_random = tcase_create("Search random element");
    tcase_add_checked_fixture(tc_search_random, setup_data, teardown_data);
    tcase_add_test(tc_search_random, test_search_random_key);
    suite_add_tcase(s, tc_search_random);

    return s;
}


static Suite* make_suite_copy(void)
{
    Suite* s = suite_create("Copy");

    TCase* tc_no_copy_func = tcase_create("Dependence KeyVal with no user copy function");
    tcase_add_checked_fixture(tc_no_copy_func, setup, teardown);
    tcase_add_test(tc_no_copy_func, test_no_copy_dependent_from_source);
    suite_add_tcase(s, tc_no_copy_func);

    TCase* tc_copy_func = tcase_create("Independence KeyVal with user copy function");
    tcase_add_test(tc_copy_func, test_copy_independent_from_source);
    suite_add_tcase(s, tc_copy_func);

    return s;
}


/* ---------- tests run ---------------------------------------------------- */


int main(void)
{
    //log_set_quiet(true);
    log_set_level(LOG_LEVEL);
    log_info("Test hashmap starting!");


    Suite
        *suite_core_hashmap    = suite_create("Test HashMap"),
        *suite_create_hashmap  = make_suite_create(),
        *suite_destroy_hashmap = make_suite_destroy(),
        *suite_clear_hashmap   = make_suite_clear(),
        *suite_insert_hashmap  = make_suite_insert(),
        *suite_remove_hashmap  = make_suite_remove(),
        *suite_search_hashmap  = make_suite_search(),
        *suite_copy_hashmap    = make_suite_copy();

    SRunner *sr = srunner_create(suite_core_hashmap);
    srunner_add_suite(sr, suite_create_hashmap);
    srunner_add_suite(sr, suite_destroy_hashmap);
    srunner_add_suite(sr, suite_clear_hashmap);
    srunner_add_suite(sr, suite_insert_hashmap);
    srunner_add_suite(sr, suite_remove_hashmap);
    srunner_add_suite(sr, suite_search_hashmap);
    srunner_add_suite(sr, suite_copy_hashmap);

    srunner_run_all(sr, CK_NORMAL);
    //srunner_run(sr, "Create", NULL, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);

    srunner_free(sr);


    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
