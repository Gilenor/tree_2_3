#ifndef TREE_2_3_H__
#define TREE_2_3_H__

/******************************************************************************
 * Implementation of the 2-3 tree data structure and basic operations on her
 * operations - insert, delete, search, get_minimum, get_maximum.
 *
 * Data:    05/07/2026
 * Autor:   Semenov Sergey A.K.A. Serginiyo
 * Version: 2.2
 *****************************************************************************/

#include <stdbool.h>

/* points to an address where the true key is saved */
typedef const void * TreeKey;

typedef struct _node Node_2_3;
typedef struct _tree Tree_2_3;

typedef int      (*func_cmp_key)     (TreeKey, TreeKey);  /* function type to compare keys */
typedef TreeKey  (*func_copy_key)    (TreeKey);           /* function type to copy key_t value */
typedef void     (*func_free_key)    (TreeKey);           /* function free allocated memory and resourses */
typedef void     (*func_print_key)   (TreeKey);           /* function to print key_t value */


/******************************************************************************
 * Since the principles of operation of all functions are obvious for this
 * version of the structure, a detailed description of them is not required
 ******************************************************************************/


/**
 * @brief Creates an empty 2-3 tree.
 *
 * @param func_cmp  Key comparison function. Required.
 * 					Should return:
 * 					< 0 if the first key is less than the second;
 * 					0 if the keys are equal;
 * 					> 0 if the first key is greater than the second.
 * 
 * @param func_copy Key copy function. Optional.
 *					If NULL, the tree saves the passed pointer without copying it.
 *
 * @param func_free Key release function. Optional.
 *					If NULL, the tree does not release keys 
 * 					when nodes are removed and the tree is destroyed.
 *
 * @return A pointer to the created tree, or NULL on error.
 *
 * @note The tree should not be used after being deleted via tree_destroy().
 * @note Key ownership depends on func_copy and func_free.
 */
Tree_2_3 *       tree_create     (func_cmp_key func_cmp, func_copy_key func_copy, func_free_key func_free);
void             tree_destroy    (Tree_2_3 *tree);
void             tree_make_empty (Tree_2_3 *tree);

bool             tree_insert_key (Tree_2_3 *tree, TreeKey key);
bool             tree_remove_key (Tree_2_3 *tree, TreeKey key);
const Node_2_3 * tree_search_key (const Tree_2_3 *tree, TreeKey key);

void             node_print      (const Node_2_3 *node, func_print_key print_key);
void             tree_print      (const Tree_2_3 *tree, func_print_key print_key);

bool             tree_is_empty       (const Tree_2_3 *tree);
TreeKey          tree_get_min        (const Tree_2_3 *tree);
TreeKey          tree_get_max        (const Tree_2_3 *tree);
const Node_2_3 * tree_get_root       (const Tree_2_3 *tree);
int              tree_height         (const Tree_2_3 *tree);
int              tree_count_elements (const Tree_2_3 *tree);

TreeKey          node_get_key    (const Node_2_3 *node);

#endif
