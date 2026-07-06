#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hashmap.h"

#include "log/log.h"


#define SIZE_ARR(arr)   (int)(sizeof(arr) / sizeof(*arr))


struct memory_counter
{
    unsigned free;
    unsigned alloc;
} *g_memory_counter;  /* use for count copy/free function calls */

static HashMap *_hashmap;  /* global object for test cases */


/* ---------- key/value functions ------------------------------------------ */

typedef struct _position
{
    int x;
    int y;
} Position;


Position position_new(int x, int y)
{ 
    return (Position){x, y}; 
}


static int compare_kv(const KeyVal *a, const KeyVal *b)
{
    log_trace("%s", __func__);
    
    Position *pos_a = KEY(a);
    Position *pos_b = KEY(b);
    int res = pos_a->x - pos_b->x;
    

    if (res == 0)
        res = pos_a->y - pos_b->y;

    return res;
}


static KeyVal copy_kv(const KeyVal *key)
{
    log_trace("%s", __func__);

    KeyVal res = {
        .key = malloc(sizeof(Position)),
        .value = malloc(sizeof(Position)),
    };


    if (!res.key || !res.value)
    {
        log_fatal("Can't allocate required memory to copy KeyVal!");
        exit(EXIT_FAILURE);
    }

    memcpy(res.key, key->key, sizeof(Position));
    memcpy(res.value, key->value, sizeof(Position));
    
    return res;
}


static void free_kv(const KeyVal *key)
{
    log_trace("%s", __func__);

    if (!key)
    {
        log_warn("Try free NULL key!");
        return;
    }
    
    free((void*)KEY(key));
    free((void*)VALUE(key));
    //free((void*)key);
}


static void print_kv(const KeyVal *key)
{
    log_trace("%s", __func__);

    Position *k = KEY(key);
    Position *v = VALUE(key);
    
    printf("Key: (%d, %d), value: (%d, %d)", k->x, k->y, v->x, v->y);
}




/* ---------- auxiliary functions ------------------------------------------ */


/* ---------- test cases --------------------------------------------------- */


/* ---------- suites ------------------------------------------------------- */


/* ---------- tests run ---------------------------------------------------- */

int main(void)
{
    //log_set_quiet(false);
    log_set_level(LOG_LEVEL);
    log_info("Test hashmap starting!");

    HashMap *hashmap = hm_create(compare_kv, NULL, NULL);
    //HashMap hashmap = hm_create(compare_kv, NULL, free_kv);
    //HashMap hashmap = hm_create(compare_kv, copy_kv, NULL);
    //HashMap hashmap = hm_create(compare_kv, copy_kv, free_kv);
    
    Position path[] = {
        {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}
    };
    
  

    for (int i = 1; i < SIZE_ARR(path); i++)
    {
        Position* key = &path[i];
        Position* value = &path[i-1];
        
        hm_insert_kv(hashmap, (HmKey)key, (HmValue)value);
    }
    
    hm_print(hashmap, print_kv);
    hm_destroy(hashmap);

    sleep(1);

    return 0;
}
