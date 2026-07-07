#pragma once

#include <stdbool.h>


#define KEY_VAL(k, v)      ((KeyVal){ .key=k, .value=v })
#define KEY(key_val)       (key_val->key)
#define VALUE(key_val)     (key_val->value)



typedef void * HmKey;
typedef void * HmValue;

typedef struct _hashmap HashMap;

typedef struct 
{
    HmKey key;
    HmValue value;
} KeyVal;


typedef int      (*func_cmp_kv)     (const KeyVal*, const KeyVal*);   /* function type to compare keys */
typedef KeyVal   (*func_copy_kv)    (const KeyVal*);                  /* function type to copy KeyVal value */
typedef void     (*func_free_kv)    (const KeyVal*);                  /* function free allocated memory and resourses */
typedef void     (*func_print_kv)   (const KeyVal*);                  /* function to print KeyVal value */


/**
 * @brief Creates an empty hashmap.
 *
 * @param func_cmp  KeyVal comparison function. Required.
 *                  Should return:
 *                  < 0 if the first key is less than the second;
 *                  0 if the keys are equal;
 *                  > 0 if the first key is greater than the second.
 * 
 * @param func_copy KeyVal copy function. Optional.
 *                  If NULL, the tree saves the passed pointer without copying it.
 *
 * @param func_free KeyVal release function. Optional.
 *                  If NULL, the hashmap does not release KeyVal 
 *                  when nodes are removed and the hashmap is destroyed.
 *
 * @return A pointer to the created hashmap, or NULL on error.
 *
 * @note The hashmap should not be used after being deleted via hm_destroy().
 * @note Key ownership depends on func_copy and func_free.
 */
HashMap * hm_create     (func_cmp_kv func_cmp, func_copy_kv func_copy, func_free_kv func_free);
void      hm_destroy    (HashMap **hashmap);
void      hm_clear      (HashMap *hashmap);
 
bool      hm_insert_kv  (HashMap *hashmap, HmKey key, HmValue value);
bool      hm_remove_key (HashMap *hashmap, HmKey key);
HmValue   hm_get_value  (const HashMap *hashmap, HmKey key);

bool      hm_is_contain (const HashMap *hashmap, HmKey key);
int       hm_length     (const HashMap *hashmap);

void hm_print (const HashMap *hashmap, func_print_kv print_key); /* Print hashmap. Need to pass a custom function to print the key/value */
