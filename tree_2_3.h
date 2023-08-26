#ifndef TREE_2_3_H__
#define TREE_2_3_H__

/******************************************************************************
 * Implementation of the 2-3 tree data structure and basic operations on her
 * operations - insert, delete, search, get_minimum, get_maximum.
 * 
 * At the moment, it only works with numeric data types.
 * In order to store a value of any type in the tree,
 * need to implement functions for copying the value, comparing and deleting,
 * implement 'value_t' as 'void *' and when creating a new tree,
 * pass pointers to these functions as well. Functions need to defined 
 * in custom code for the data type you are using.
 * 
 * Data:	26/08/2023
 * Autor:	Semenov Sergey
 * Version:	1.0
 *****************************************************************************/

#include <stdbool.h>

typedef int value_t;


enum nodetype
{
	EMPTY,
	INNER,
	LEAF
};


typedef struct node
{
	enum nodetype type;

	union
	{
		/* if nodetype is INNER */
		struct
		{
			struct node *first;
			struct node *second;
			struct node *third;

			value_t second_min;
			value_t third_min;
		};

		/* if nodetype is LEAF */
		value_t value;
	};

} NODE_2_3, *TREE_2_3;


/******************************************************************************
 * Since the principles of operation of all functions are obvious for this 
 * version of the structure, a detailed description of them is not required
 ******************************************************************************/

TREE_2_3 		new_tree		(void);
void 			tree_delete		(TREE_2_3 tree);

void 			insert_value	(TREE_2_3 *tree, value_t value);
void            remove_value    (TREE_2_3 *tree, value_t value);
struct node * 	search_value	(TREE_2_3  tree, value_t value);

void            print_node      (struct node *node);
void            print_tree      (TREE_2_3 tree);

bool            tree_is_empty       (TREE_2_3 tree);
value_t         tree_get_min        (TREE_2_3 tree);
value_t         tree_get_max        (TREE_2_3 tree);
int             tree_height         (TREE_2_3 tree);
int             tree_count_elements (TREE_2_3 tree);


#endif
