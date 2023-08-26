#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tree_2_3.h"

#define DELETE_CORRECT  NULL


/* -------- static functions ----------------------------------------------- */


/* return true if 'node a' bigger than 'node b'
 * if node be  NULL he was considered greater than others 
 * when sorting, all empty nodes will be on the "right" */
static bool bigger_than(struct node *a, struct node *b)
{
    if (a && b)
        return get_min(a) > get_min(b);
    else
    if (!a && b) // only 'a is Null
        return true;

    return false;
}


/* sort elements to ascending order 
 * if (*a > *b) they switch places */
static void min_max(struct node **a, struct node **b)
{
    struct node *tmp;

    if (bigger_than(*a, *b))
    {
        tmp = *a;
        *a = *b;
        *b = tmp;
    }
}


/* count children of node */
static int child_cnt(NODE_2_3 *node)
{
    if (node == NULL)
        return 0;

    return (node->first != NULL) + (node->second != NULL) + (node->third != NULL);
}


/* return minimal element in node/tree */
static value_t get_min(struct node *node)
{
    // TODO: what do in this way, maybe return pointer to value?
    if (node == NULL)
    {
        fputs("Error! The node does not exist! Can't get min value.", stderr);
        exit(EXIT_FAILURE);
    }

    while (node->type != LEAF)
        node = node->first;

    return node->value;
}


static struct node * new_empty_node(void)
{
	struct node *tmp = calloc(sizeof(struct node), 1);

	if (tmp == NULL)
		exit(EXIT_FAILURE);

	return tmp;
}


static struct node * new_inner_node(void)
{
    struct node *tmp = new_empty_node();

    tmp->type = INNER;

    return tmp;
}


static struct node * new_leaf_node(value_t value)
{
	struct node *tmp = new_empty_node();

	tmp->type = LEAF;
	tmp->value = value;

	return tmp;
}


/* sorted node in asc order and make it valid */
static void validate_node(struct node *node)
{
    // TODO: maybe error or exception?
    if (node == NULL)
        return;

    min_max(&node->first, &node->second);

    if (node->third != NULL)
    {
        min_max(&node->first, &node->third);
        min_max(&node->second, &node->third);
    }

    node->second_min = get_min(node->second);
    node->third_min  = get_min(node->third);
}


/* Added node to old_node and made his valid
 * If old_node has place, his "merge" with added, validated and return NULL.
 * Else the two smallest elements are placed in the old node,
 * and the two largest elements are placed in the new node.
 * After that, the new node pops up further recursively.   */
static struct node * update_node(struct node *old_node, struct node *added)
{
    if (old_node->third == NULL)
    {
        old_node->third = added;
        validate_node(old_node);

        return NULL;
    }

    struct node *new_node = new_inner_node();

    new_node->second = old_node->third;
    old_node->third = NULL;

    /* the two smallest elements remain in the old node,
       and the two largest go to the new node */
    if (bigger_than(added, old_node->second))
        new_node->first = added;
    else
    {
        new_node->first = old_node->second;
        old_node->second = added;

        validate_node(old_node);
    }

    validate_node(new_node);

    /* return the "larger" of the nodes */
    return new_node;
}


/* merge old root and added node in new root of tree */
static void update_root(TREE_2_3 *tree, struct node *added)
{
    struct node *new_root = new_inner_node();

    new_root->first = *tree;
    new_root->second = added;
    validate_node(new_root);

    *tree = new_root;
}


static void delete_child(TREE_2_3 tree, NODE_2_3 *node, enum nodetype type)
{
    if (tree->first == node)
        tree->first = NULL;
    else
    if (tree->second == node)
        tree->second = NULL;
    else
    if (tree->third == node)
        tree->third = NULL;

    if (type == LEAF)
        free(node);
}


static void insert_child(TREE_2_3 tree, NODE_2_3 *child)
{
    if (child_cnt(tree) == 1)
    {
        tree->second = update_node(tree->first, child);
    }
    else
    if (child_cnt(tree) == 2)
    {
        if (bigger_than(child, tree->second))
            tree->third = update_node(tree->second, child);
        else
            tree->third = update_node(tree->first, child);
    }
    else
    {
        fprintf(stderr, "Error! Can't add child, node is full! Line %d", __LINE__);
        exit(EXIT_FAILURE);
    }
    validate_node(tree);
}


static struct node * delete_value(TREE_2_3 tree, value_t value)
{
    /* value not in tree */
    if (tree == NULL)
        return NULL;

    /* value finded */
    if ( tree->type == LEAF && value == tree->value)
        return tree;


    struct node *deleted = NULL;

    /* try find value in tree */
    if (value < tree->second_min)
        deleted = delete_value(tree->first, value);
    else
    if ( !tree->third || (value < tree->third_min) )
        deleted = delete_value(tree->second, value);
    else
        deleted = delete_value(tree->third, value);

    /* when deleting a value results in an incorrect node (with one child)
       We'll try to merge it with one of the brothers, and do it recursively */
    if (deleted)
    {
        switch (deleted->type)
        {
            case LEAF:
                        delete_child(tree, deleted, LEAF);
                        validate_node(tree);

                        if (child_cnt(tree) == 1)
                            return tree;
                        break;

            case INNER:
                        delete_child(tree, deleted, INNER);
                        validate_node(tree);
                        insert_child(tree, deleted->first);
                        free(deleted);

                        if (child_cnt(tree) == 1)
                            return tree;
                        break;

            case EMPTY:
                        fprintf(stderr, "Error! Tree can't have empty node! Line %d", __LINE__);
            default:
                        exit(EXIT_FAILURE);
        }
    }

    return DELETE_CORRECT;
}


static struct node * add_value(TREE_2_3 tree, value_t value)
{
    if (tree == NULL)
        return new_leaf_node(value);

    if (tree->type == LEAF)
    {
        if (value == tree->value)
            return NULL;  // value in tree, don't duplicated
        else
            return new_leaf_node(value); // value not in tree
    }


    struct node *new_node = NULL;
    struct node *result = NULL;


    if (value < tree->second_min)
        new_node = add_value(tree->first, value);
    else
    if ( !tree->third || (value < tree->third_min) )
        new_node = add_value(tree->second, value);
    else
        new_node = add_value(tree->third, value);

    /* If a new node is created when adding an item to children,
       add this node to the parent and do it recursively */
    if (new_node != NULL)
        result = update_node(tree, new_node);

    return result;
}


static void print_tree_elements_in_order(TREE_2_3 tree,  int *num_element)
{
    if (tree == NULL)
        return;

    if (tree->type == LEAF)
    {
        (*num_element)++;
        printf("%d) %d\n", *num_element, tree->value);

        return;
    }

    print_elements_in_tree(tree->first, num_element);
    print_elements_in_tree(tree->second, num_element);
    print_elements_in_tree(tree->third, num_element);
}


/* ------------------------------------------------------------------------- */


TREE_2_3 new_tree(void)
{
	return NULL;
}


void insert_value(TREE_2_3 *tree, value_t value)
{
	/* Empty tree */
	if (*tree == NULL)
		*tree = new_leaf_node(value);
    else 
    if ( (*tree)->type == LEAF ) /* Tree is leaf (1 element) */
    {
        struct node *new_node = new_inner_node();

        new_node->first = *tree;
        new_node->second = new_leaf_node(value);

        *tree = new_node;

        validate_node(*tree);
    }
    else /* Element not in tree */
	{
		struct node *new_node = add_value(*tree, value);

        /* need to update the root of tree */
		if (new_node != NULL)
            update_root(tree, new_node);
	}
}


void remove_value(TREE_2_3 *tree, value_t value)
{
    /* Empty tree */
	if (*tree == NULL)
		return;


    struct node *tmp = NULL;
    struct node *deleted = delete_value(*tree, value);

    /* need to update the root of tree
     * because he had only one child left */
    if (deleted != NULL)
    {
        if ( (*tree)->type == INNER)
        {
            tmp = (*tree)->first;
            free(*tree);
            *tree = tmp;
        }
        else
        {
            free(*tree);
            *tree = NULL;
        }
    }
}


/* return addres leaf with value or null if value not found */
struct node * search_value(TREE_2_3 tree, value_t value)
{
	if (tree == NULL)
		return NULL;

	switch (tree->type)
	{
		case LEAF:
					if (tree->value == value)
						return tree;
					return NULL;

		case INNER:
					if (value < tree->second_min)
						return find_value(&tree->first, value);
					else
					if ( !tree->third || (value < tree->third_min) )
						return find_value(&tree->second, value);
					else
						return find_value(&tree->third, value);

		case EMPTY:
					fprintf(stderr, "Error! Tree can't have empty node!");
        default:
					exit(EXIT_FAILURE);
	}
}


void print_node(struct node *node)
{
    if (node == NULL)
    {
        puts("Node is empty!");
        return;
    }

    switch(node->type)
    {
        case LEAF:
                    puts("Node is leaf");
                    printf("Adr: %p\n", node);
                    printf("Val: %d\n", node->value);
                    break;

        case INNER:
                    puts("Node is inner");
                    printf("Adr: %p\n", node);
                    printf("First adr: %p\n", node->first);
                    printf("Second adr: %p\n", node->second);
                    printf("Third adr: %p\n", node->third);

                    printf("Min second: %d\n", node->second_min);
                    if (node->third)
                        printf("Min third: %d\n", node->third_min);

                    break;

        case EMPTY:
                    puts("Node is empty");
                    printf("Adr: %p\n", node);

        default:
                    printf("Error! Incorrect node!");
                    break;
    }
}


void print_tree(TREE_2_3 tree)
{
    int num_element = 0;

    print_tree_elements_in_order(tree, &num_element);
}


bool tree_is_empty(TREE_2_3 tree)
{
    return tree_count_elements(tree) == 0; // tree == NULL;
}


/* count of elements(leafs) in tree */
int tree_count_elements(TREE_2_3 tree)
{
    if (tree == NULL)
        return 0;

    if (tree->type == LEAF)
        return 1;

    int cnt = 0;

    cnt += tree_count_elements(tree->first);
    cnt += tree_count_elements(tree->second);
    cnt += tree_count_elements(tree->third);

    return cnt;
}


/* return height of tree */
int tree_height(TREE_2_3 tree)
{
    int height = 0;

    if (tree == NULL)
        return height;

    while (tree->type != LEAF)
    {
        height++;
        tree = tree->first;
    }

    return height;
}


value_t tree_get_min(TREE_2_3 tree)
{
    if (tree == NULL)
    {
        fputs("Can't find min value in empty tree. Error!", stderr);
        exit(EXIT_FAILURE);
    }

    return get_min(tree);
}


value_t tree_get_max(TREE_2_3 tree)
{
    if (tree == NULL)
    {
        fputs("Can't find max value in empty tree. Error!", stderr);
        exit(EXIT_FAILURE);
    }

    struct node *current = tree;

    while (current->type != LEAF)
    {
        if (child_cnt(current) == 3)
            current = current->third;
        else
        if (child_cnt(current) == 2)
            current = current->second;
        else
        {
            fputs("Inner node with one child not valid! Error!", stderr);
            exit(EXIT_FAILURE);
        }
    }

    return current->value;
}


/* first free children of tree, than free tree*/
void tree_delete(TREE_2_3 tree)
{
	if (tree == NULL)
		return;

	switch (tree->type)
	{
		case LEAF:
					break;

		case INNER:
					tree_delete(tree->first);
					tree_delete(tree->second);
					tree_delete(tree->third);
					break;

		case EMPTY:
					fprintf(stderr, "Error! Tree can't have empty node!");
					exit(EXIT_FAILURE);
	}

	free(tree);
}
