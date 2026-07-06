#include <stdbool.h>
#include <stdlib.h>

#include "hashmap.h"

#include "tree_2_3/tree_2_3.h"
#include "log/log.h"


struct _hashmap
{
    Tree_2_3 *tree;
    func_copy_kv copy_kv;
    func_free_kv free_kv;
};

/* для связывания hashmap в виду остутствия замыканий и методов */
typedef struct _hm_context
{
    KeyVal kv;
    const HashMap *hm;
} HmContext;


static TreeKey _copy_kv (HmContext* context);
static void    _free_kv (HmContext* context);


HashMap * hm_create(func_cmp_kv cmp_kv, func_copy_kv copy_kv, func_free_kv free_kv)
{
    log_trace("%s", __func__);
    
    if (!cmp_kv)
    {
        log_error("Error! Need to state key/value functions --> [Necessarily: compare]; [Optional: copy, free]");
        exit(EXIT_FAILURE);
    }
    
    Tree_2_3 *tree = tree_create((func_cmp_key)cmp_kv, (func_copy_key)_copy_kv, (func_free_key)_free_kv);
    HashMap *hashmap = malloc(sizeof(*hashmap));

    if (!tree || !hashmap)
    {
        log_warn("Can't allocate required memory to create hashmap!");
        return NULL;
    }

    hashmap->tree = tree;
    hashmap->copy_kv = copy_kv;
    hashmap->free_kv = free_kv;

    return hashmap;
}


void hm_destroy(HashMap *hashmap)
{
    log_trace("%s", __func__);

    tree_destroy(hashmap->tree);
    free(hashmap);
}


void hm_clear(HashMap *hashmap)
{
    log_trace("%s", __func__);

    tree_make_empty(hashmap->tree);
}


bool hm_insert_kv(HashMap *hashmap, HmKey key, HmValue value)
{
    log_trace("%s", __func__);

    HmContext kv = {
        .kv = { .key=key, .value=value },
        .hm = hashmap,
    };


    if (tree_search_key(hashmap->tree, (TreeKey)&kv))
    {
        log_debug("Change value for existing key in hashmap");
        // подумать, можно ли безопасно изменить только значение для ключа
        tree_remove_key(hashmap->tree, (TreeKey)&kv);
    }
    
    return tree_insert_key(hashmap->tree, (TreeKey)&kv);
}


bool hm_remove_key(HashMap *hashmap, HmKey key)
{
    log_trace("%s", __func__);

    return tree_remove_key(hashmap->tree, (TreeKey)key);
}


HmValue hm_get_value(const HashMap *hashmap, HmKey key)
{
    log_trace("%s", __func__);
    
    KeyVal kv = (KeyVal){ .key=key };
    const Node_2_3 *finded = tree_search_key(hashmap->tree, (TreeKey)&kv);
    

    if (finded != NULL)
    {
        TreeKey t_key = node_get_key(finded);
        return ((KeyVal*)t_key)->value;
    }

    return NULL;
}


bool hm_is_contain(const HashMap *hashmap, HmKey key)
{
    log_trace("%s", __func__);
    
    KeyVal kv = { .key=key };
    const Node_2_3 *finded = tree_search_key(hashmap->tree, (TreeKey)&kv);
    
    return finded != NULL;
}


int hm_length(const HashMap *hashmap)
{
    log_trace("%s", __func__);

    return tree_count_elements(hashmap->tree);
}


void hm_print(const HashMap *hashmap, func_print_kv print_key)
{
    log_trace("%s", __func__);
    
    tree_print(hashmap->tree, (func_print_key)print_key);
}


/* ---------- static functions ---------------------------------------------- */


static TreeKey _copy_kv(HmContext* context)
{
    log_trace("%s", __func__);

    HmContext* key = malloc(sizeof(HmContext));


    if (!key)
    {
        log_fatal("Can't allocate required memory for KeyVal!");
        exit(EXIT_FAILURE);
    }

    const HashMap *hm = context->hm;
    KeyVal kv = context->kv;

    if (hm->copy_kv)
        key->kv = hm->copy_kv(&kv);
    else
        key->kv = kv;

    key->hm = hm;

    return (TreeKey)key;
}


static void _free_kv(HmContext* context)
{
    log_trace("%s", __func__);

    const HashMap *hm = context->hm;
    KeyVal kv = context->kv;


    if (hm->free_kv)
        hm->free_kv(&kv);

    free(context);
}
