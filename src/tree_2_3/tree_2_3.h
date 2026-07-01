#ifndef TREE_2_3_H__
#define TREE_2_3_H__

/******************************************************************************
 * Implementation of the 2-3 tree data structure and basic operations on her
 * operations - insert, delete, search, get_minimum, get_maximum.
 *
 * To work with the structure, you need to define and pass
 * to it the following user-defined functions for working with the key:
 *
 * 
 * func_cmp_key - <REQUIRED>
 *                <DEFINITION>
 *                compares the values of two keys and, depending on the result,
 *                returns one of the values defined in enum compare_t.
 *
 *                <ARGUMENTS>
 *                the compared keys are passed as a function parameter
 *
 *                <RETURN VALUE>
 *                must follow the comparison convention for the return value: 
 *                    0 - if the elements are equal
 *                   <0 - when the first is less than the second
 *                   >0 - when the first is greater than the second
 *
 *                <ATTENTION>
 *                this parameter is REQUIRED, without it it is impossible to create a tree
 *
 * 
 * func_copy_key - <OPTIONAL/NULLABLE>
 *                 <DEFINITION>
 *                 allocates memory and copies the value of the key into it
 *                 if NULL is passed, the key will not be copied, but simply stored at the passed address
 *
 *                 <ARGUMENTS>
 *                 key whose value to copy
 *
 *                 <RETURN VALUE>
 *                 created copy of the key
 *
 *                 <ATTENTION>
 *                 type TreeKey the address where the value is stored
 *                 think carefully what value you want to keep!
 *                 this is entirely the responsibility of the user,
 *                 because any work with the tree_type falls on user functions.
 * 
 *                 if the value is specified as NULL, then the value of the passed TreeKey pointer
 *                 will simply be stored, which can violate the invariant of the tree as a sorted data structure, 
 *                 which will cause UNDEFINED BEHAVIOR!!!
 *
 * 
 * func_free_key - <OPTIONAL/NULLABLE>
 *                 <DEFINITION>
 *                 frees the memory allocated for the key
 *                 if NULL is passed, the key will not be free
 *
 *                 <ARGUMENTS>
 *                 key to be removed
 * 
 *                 <ATTENTION>
 *                 if the value is specified as NULL, 
 *                 then the responsibility for freeing the resources allocated for TreeKey 
 *                 falls on the shoulders of the caller
 *
 *
 * func_print_key - <DEFINITION>
 *                  prints the key value without newline
 *
 *                  <ARGUMENTS>
 *                  key to be printed
 *
 *
 * Data:    25/06/2026
 * Autor:   Semenov Sergey
 * Version: 2.1
 *****************************************************************************/

#include <stdbool.h>

/* points to an address where the true key is saved */
typedef const void * TreeKey;

typedef struct _node *Node_2_3;
typedef struct _tree *Tree_2_3;

typedef int      (*func_cmp_key)     (TreeKey, TreeKey);  /* function type to compare keys */
typedef TreeKey  (*func_copy_key)    (TreeKey);           /* function type to copy key_t value */
typedef void     (*func_free_key)    (TreeKey);           /* function free allocated memory and resourses */
typedef void     (*func_print_key)   (TreeKey);           /* function to print key_t value */


/******************************************************************************
 * Since the principles of operation of all functions are obvious for this
 * version of the structure, a detailed description of them is not required
 ******************************************************************************/

Tree_2_3        tree_create     (func_cmp_key, func_copy_key, func_free_key);
void            tree_destroy    (Tree_2_3 tree);
void            tree_make_empty (Tree_2_3 tree);

bool            insert_key      (Tree_2_3 tree, TreeKey key);
bool            remove_key      (Tree_2_3 tree, TreeKey key);
Node_2_3        search_key      (Tree_2_3 tree, TreeKey key);

void            node_print      (Node_2_3 node, func_print_key print_key);
void            tree_print      (Tree_2_3 tree, func_print_key print_key);

bool            tree_is_empty       (Tree_2_3 tree);
TreeKey         tree_get_min        (Tree_2_3 tree);
TreeKey         tree_get_max        (Tree_2_3 tree);
Node_2_3        tree_get_root       (Tree_2_3 tree);
int             tree_height         (Tree_2_3 tree);
int             tree_count_elements (Tree_2_3 tree);

TreeKey         node_get_key    (Node_2_3 node);


#endif
