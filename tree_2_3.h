#ifndef TREE_2_3_H__
#define TREE_2_3_H__

/******************************************************************************
 * Implementation of the 2-3 tree data structure and basic operations on her
 * operations - insert, delete, search, get_minimum, get_maximum.
 *
 * To work with the structure, you need to define and pass
 * to it the following user-defined functions for working with the key:
 *
 * func_cmp_key - <DEFINITION>
 *                compares the values of two keys and, depending on the result,
 *                returns one of the values defined in enum compare_t.
 *
 *                <ARGUMENTS>
 *                the compared keys are passed as a function parameter
 *
 *                <RETURN VALUE>
 *                must strictly belong to one of the values defined in enum compare_t
 *
 *                <ATTENTION>
 *                be careful when using comparison functions,
 *                they must return only one of the specified values!!!
 *                functions like strcmp() return values <0, 0, >0,
 *                which are not strictly defined!
 *
 * func_copy_key - <DEFINITION>
 *                 allocates memory and copies the value of the key into it
 *
 *                 <ARGUMENTS>
 *                 key whose value to copy
 *
 *                 <RETURN VALUE>
 *                 created copy of the key
 *
 *                 <ATTENTION>
 *                 type tree_key the address where the value is stored
 *                 think carefully what value you want to keep!
 *                 this is entirely the responsibility of the user,
 *                 because any work with the tree_type falls on user functions.
 *
 * func_free_key - <DEFINITION>
 *                 frees the memory allocated for the key
 *
 *                 <ARGUMENTS>
 *                 key to be removed
 *
 *
 * func_print_key - <DEFINITION>
 *                  prints the key value without newline
 *
 *                  <ARGUMENTS>
 *                  key to be printed
 *
 *
 * Data:	29/08/2023
 * Autor:	Semenov Sergey
 * Version:	2.0
 *****************************************************************************/

#include <stdbool.h>

/* points to an address where the true key is saved */
typedef const void * tree_key;

typedef struct node *Node_2_3;
typedef struct tree *Tree_2_3;



/* admissible values to return from the comparison function */
enum compare_t
{
    EQUAL   =  0,
    GREATER =  1,
    LESS    = -1,
};

typedef int         (*func_cmp_key)     (tree_key, tree_key);   /* function type to compare keys */
typedef tree_key    (*func_copy_key)    (tree_key);             /* function type to copy key_t value */
typedef void	    (*func_free_key)    (tree_key);             /* function free allocated memory and resourses */
typedef void        (*func_print_key)   (tree_key);   			/* function to print key_t value */


/******************************************************************************
 * Since the principles of operation of all functions are obvious for this
 * version of the structure, a detailed description of them is not required
 ******************************************************************************/

Tree_2_3 		new_tree		(func_cmp_key, func_copy_key, func_free_key);
void 			tree_delete		(Tree_2_3 tree);
void            tree_make_empty (Tree_2_3 tree);

void 			insert_key	    (Tree_2_3 tree, tree_key key);
void            remove_key      (Tree_2_3 tree, tree_key key);
struct node * 	search_key	    (Node_2_3 root, tree_key key);

void            print_node      (Node_2_3 node, func_print_key print_key);
void            print_tree      (Tree_2_3 tree, func_print_key print_key);

bool            tree_is_empty       (Tree_2_3 tree);
tree_key        tree_get_min        (Tree_2_3 tree);
tree_key        tree_get_max        (Tree_2_3 tree);
struct node *   tree_get_root       (Tree_2_3 tree);
int             tree_height         (Tree_2_3 tree);
int             tree_count_elements (Tree_2_3 tree);


#endif
