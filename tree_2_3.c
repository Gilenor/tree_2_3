#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tree_2_3.h"

#include "log/log.h"


#define DELETE_CORRECT  NULL


/* -------- Data structs --------------------------------------------------- */


enum nodetype
{
    EMPTY,
    INNER,
    LEAF
};


struct _node
{
    enum nodetype type;

    union
    {
        /* If nodetype is INNER */
        struct
        {
            struct _node *first;
            struct _node *second;
            struct _node *third;

            TreeKey second_min;
            TreeKey third_min;
        };

        /* If nodetype is LEAF */
        struct
        {
            TreeKey key;

            /* To access the function working with the key
               it is possible to use a pointer to the tree
               where they are contained, but this neither
               reduces memory usage nor makes it more clear */
            func_cmp_key    cmp_key;
            func_copy_key   copy_key;
            func_free_key   free_key;
        };
    };
};


struct _tree
{
    struct _node *root;
    unsigned long elements;

    /* Functions for working with key value */
    func_cmp_key    cmp_key;
    func_copy_key   copy_key;
    func_free_key   free_key;
};


/* -------- Static functions ----------------------------------------------- */


/* Return address smaller node in node/tree */
static struct _node * get_min_node(Node_2_3 root)
{
    log_trace("%s", __func__);

    if (root == NULL)
        return NULL;

    while (root->type != LEAF)
        root = root->first;

    return root;
}


/* Return address smaller key for root */
static TreeKey get_min(Node_2_3 root)
{
    log_trace("%s", __func__);

    struct _node *result = get_min_node(root);

    if (!result)
        return NULL;

    return result->key;
}


/* Getter for comparing function associated with TreeKey type */
static func_cmp_key get_cmp_func(struct _node *node)
{
    log_trace("%s", __func__);

    struct _node *leaf = get_min_node(node);

    return leaf->cmp_key;
}


/* Getter for copying function associated with TreeKey type */
static func_copy_key get_copy_func(struct _node *node)
{
    log_trace("%s", __func__);

    struct _node *leaf = get_min_node(node);

    return leaf->copy_key;
}


/* Getter for freeing function associated with TreeKey type */
static func_free_key get_free_func(struct _node *node)
{
    log_trace("%s", __func__);

    struct _node *leaf = get_min_node(node);

    return leaf->free_key;
}


/* Return true if <node a> bigger than <node b>
 * if node be  NULL he was considered greater than others
 * when sorting, all empty nodes will be on the "right" */
static bool bigger_than(struct _node *a, struct _node *b)
{
    log_trace("%s", __func__);

    if (a && b)
        return GREATER == get_cmp_func(a)(get_min(a), get_min(b));
    else
    if (!a && b) // only <a> is Null
        return true;

    return false;
}


/* Sort elements to ascending order
 * if (*a > *b) they switch places */
static void min_max(struct _node **a, struct _node **b)
{
    log_trace("%s", __func__);

    struct _node *tmp;

    if (bigger_than(*a, *b))
    {
        tmp = *a;
        *a = *b;
        *b = tmp;
    }
}


/* Count children of node */
static int child_cnt(struct _node *node)
{
    log_trace("%s", __func__);

    if (node == NULL)
        return 0;

    return (node->first != NULL) + (node->second != NULL) + (node->third != NULL);
}


/* Make and return node with EMPTY type */
static struct _node * new_empty_node(void)
{
    log_trace("%s", __func__);

    struct _node *tmp = calloc(sizeof(struct _node), 1);

    if (tmp == NULL)
        exit(EXIT_FAILURE);

    return tmp;
}


/* Make and return node with INNER type */
static struct _node * new_inner_node(void)
{
    log_trace("%s", __func__);

    struct _node *tmp = new_empty_node();

    tmp->type = INNER;

    return tmp;
}


/* Make and return node with LEAF type */
static struct _node * new_leaf_node(TreeKey value, Tree_2_3 tree)
{
    log_trace("%s", __func__);

    struct _node *tmp = new_empty_node();

    tmp->type = LEAF;
    
    /* if the copy function is defined, then a copy of the element is made
     * otherwise the element is simply stored as an pointer */
    if (tree->copy_key)
        tmp->key = tree->copy_key(value);
    else
        tmp->key = value;

    /* Each leaf contains pointers to functions
       for working with the key */
    tmp->cmp_key  = tree->cmp_key;
    tmp->copy_key = tree->copy_key;
    tmp->free_key = tree->free_key;

    return tmp;
}


/* Releases resources allocated for the key */
static void free_key(struct _node *node)
{
    log_trace("%s", __func__);

    func_free_key free_fn = get_free_func(node);
    
    if (free_fn)
        free_fn(node->key);
}


/* Releases resources allocated for the node and key */
static void free_node(struct _node *node)
{
    log_trace("%s", __func__);

    free_key(node);
    free(node);
}


/* Sorted node in asc order and make it valid */
static void validate_node(struct _node *node)
{
    log_trace("%s", __func__);

    if (node == NULL)
    {
        log_error("Try work with nullable node!");
        exit(EXIT_FAILURE);
    }

    min_max(&node->first, &node->second);

    if (node->third != NULL)
    {
        min_max(&node->first, &node->third);
        min_max(&node->second, &node->third);
    }

    if (get_min(node->second))
        node->second_min = get_min(node->second);

    if (get_min(node->third))
        node->third_min  = get_min(node->third);
}


/* Added node to old_node and made his valid
 * If old_node has place, his "merge" with added, validated and return NULL.
 * Else the two smallest elements are placed in the old node,
 * and the two largest elements are placed in the new node.
 * After that, the new node pops up further recursively.   */
static struct _node * update_node(struct _node *old_node, struct _node *added)
{
    log_trace("%s", __func__);

    if (old_node->third == NULL)
    {
        old_node->third = added;
        validate_node(old_node);

        return NULL;
    }

    struct _node *new_node = new_inner_node();

    new_node->second = old_node->third;
    old_node->third = NULL;

    /* The two smallest elements remain in the old node,
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

    /* Return the "larger" of the nodes */
    return new_node;
}


/* Merge old root and added node in new root of tree */
static void update_root(Tree_2_3 tree, struct _node *added)
{
    log_trace("%s", __func__);

    struct _node *new_root = new_inner_node();

    new_root->first = tree->root;
    new_root->second = added;
    validate_node(new_root);

    tree->root = new_root;
}


/* Delete <child> node from <root> */
static void delete_child(Node_2_3 root, Node_2_3 node)
{
    log_trace("%s", __func__);

    if (root->first == node)
        root->first = NULL;
    else
    if (root->second == node)
        root->second = NULL;
    else
    if (root->third == node)
        root->third = NULL;

    if (node->type == LEAF)
        free_node(node);
}


/* Add child node in root */
static void add_child(Node_2_3 root, Node_2_3 child)
{
    log_trace("%s", __func__);

    if (child_cnt(root) == 1)
    {
        root->second = update_node(root->first, child);
    }
    else
    if (child_cnt(root) == 2)
    {
        if (bigger_than(child, root->second))
            root->third = update_node(root->second, child);
        else
            root->third = update_node(root->first, child);
    }
    else
    {
        log_error("Can't add child, root node is full!");
        exit(EXIT_FAILURE);
    }

    validate_node(root);
}


/* If the tree has a leaf with a value, the function deletes it
   and restores the validity of the tree on the back of the recursion  */
static struct _node * delete_value(Node_2_3 root, TreeKey value)
{
    log_trace("%s", __func__);
    
    /* Value not in tree */
    if (root == NULL)
        return NULL;
    
    struct _node *deleted = NULL;
    func_cmp_key compare = get_cmp_func(root);


    /* Value finded */
    if ( root->type == LEAF && EQUAL == compare(root->key, value) )
        return root;

    /* Try find value in tree */
    if ( LESS == compare(value, root->second_min) )
        deleted = delete_value(root->first, value);
    else
    if ( !root->third || LESS == compare(value, root->third_min) )
        deleted = delete_value(root->second, value);
    else
        deleted = delete_value(root->third, value);

    /* When deleting a value results in an incorrect node (with one child)
       they node will be merge with one of his brothers */
    if (deleted)
    {
        switch (deleted->type)
        {
            case LEAF:
                        delete_child(root, deleted);
                        validate_node(root);

                        if (child_cnt(root) == 1)
                            return root;
                        break;

            case INNER:
                        delete_child(root, deleted);
                        validate_node(root);
                        add_child(root, deleted->first);
                        free(deleted);

                        if (child_cnt(root) == 1)
                            return root;
                        break;

            case EMPTY:
                        log_error("Tree can't have empty node!");
                        exit(EXIT_FAILURE);
            default:
                        log_error("Undefined type of Node_2_3!");
                        exit(EXIT_FAILURE);
        }
    }

    return DELETE_CORRECT;
}


/* Looking for the place where the key should be
   if it is already occupied, it returns null
   otherwise, inserts the key into the tree and
   restores its validity on the back of the recursion */
static struct _node * add_value(Node_2_3 root, TreeKey value, Tree_2_3 tree)
{
    log_trace("%s", __func__);

    struct _node *result = NULL;
    struct _node *new_node = NULL;
    func_cmp_key compare = tree->cmp_key;

    /* Value not in tree */
    if (root == NULL)
        return new_leaf_node(value, tree);

    /* Find place where value must be */
    if (root->type == LEAF)
    {
        if (EQUAL == compare(value, root->key))
            return NULL;  // value in tree, don't duplicated
        else
            return new_leaf_node(value, tree); // value not in tree
    }

    /* Try find value in tree */
    if ( LESS == compare(value, root->second_min) )
        new_node = add_value(root->first, value, tree);
    else
    if ( !root->third || LESS == compare(value, root->third_min) )
        new_node = add_value(root->second, value, tree);
    else
        new_node = add_value(root->third, value, tree);

    /* If a new node is created when adding an item to children,
       add this node to the parent and do it recursively */
    if (new_node != NULL)
        result = update_node(root, new_node);

    return result;
}


/* Return addres leaf with value or null if value not found */
static struct _node * search_value(Node_2_3 root, TreeKey value)
{
    log_trace("%s", __func__);
    
    if (root == NULL)
        return NULL;

    func_cmp_key compare = get_cmp_func(root);

    switch (root->type)
    {
        case LEAF:
                    if (EQUAL == compare(root->key, value))
                        return root;
                    break;

        case INNER:
                    if (LESS == compare(value, root->second_min))
                        return search_value(root->first, value);
                    else
                    if ( !root->third || LESS == compare(value, root->third_min) )
                        return search_value(root->second, value);
                    else
                        return search_value(root->third, value);

        case EMPTY:
                    log_error("Tree can't have empty node!");
                    exit(EXIT_FAILURE);
        default:
                    log_error("Undefined type of Node_2_3!");
                    exit(EXIT_FAILURE);
    }

    /* key not found */
    return NULL;
}


/* First free children of tree, than free tree */
static void tree_free(Node_2_3 tree)
{
    log_trace("%s", __func__);

    if (tree == NULL)
        return;

    switch (tree->type)
    {
        case LEAF:
                    //get_free_func(tree)(tree->key);
                    free_key(tree);
                    break;

        case INNER:
                    tree_free(tree->first);
                    tree_free(tree->second);
                    tree_free(tree->third);
                    break;

        case EMPTY:
                    log_error("Tree can't have empty node!");
                    exit(EXIT_FAILURE);
    }

    //free_node(tree);
    free(tree);
}


/* print all elemnts in tree in ascending order */
static void print_tree_elements_in_order(Node_2_3 root, int *num_element, func_print_key print_key)
{
    log_trace("%s", __func__);

    if (root == NULL)
        return;

    if (root->type == LEAF)
    {
        (*num_element)++;
        printf("%d) ", *num_element);
        print_key(root->key);
        putchar('\n');

        return;
    }

    /* determines the order in which the tree is traversed (ascending) */
    print_tree_elements_in_order(root->first, num_element, print_key);
    print_tree_elements_in_order(root->second, num_element, print_key);
    print_tree_elements_in_order(root->third, num_element, print_key);
}


/* ------------------------------------------------------------------------- */


/** Creates an empty tree with functions to operate on the key value */
Tree_2_3 tree_create(func_cmp_key key_cmp, func_copy_key key_copy, func_free_key key_free)
{
    log_trace("%s", __func__);

    if (!key_cmp)
    {
        log_error("Need to state key functions --> [Necessarily: compare]; [Optional: copy, free]");
        exit(EXIT_FAILURE);
    }

    Tree_2_3 tmp = malloc(sizeof(*tmp));
    
    /* maybe it's better to exit with an error */
    if (!tmp)
    {
        log_warn("Failed to allocate memory for create Tree_2_3");
        return NULL;
    }

    *tmp = (struct _tree){ .cmp_key=key_cmp, .copy_key=key_copy, .free_key=key_free };

    return tmp;
}


/* Insert value in tree if it's not there */
void insert_key(Tree_2_3 tree, TreeKey value)
{
    log_trace("%s", __func__);

    if (tree == NULL)
    {
        log_error("Try work with nullable node");
        exit(EXIT_FAILURE);
    }

    /* Empty tree */
    if (tree_is_empty(tree))
    {
        tree->elements++;
        tree->root = new_leaf_node(value, tree);
    }
    else /* Tree is leaf (1 element) */
    if ( tree->root->type == LEAF )
    {
        struct _node *new_node = new_inner_node();

        /* Create a new root and add both leaves to it */
        new_node->first = tree->root;
        new_node->second = new_leaf_node(value, tree);

        tree->root = new_node;
        tree->elements++;

        validate_node(tree->root);
    }
    else /* Element not in tree */
    if (!search_key(tree, value))
    {
        struct _node *new_node = add_value(tree->root, value, tree);

        tree->elements++;

        /* Check is need to update the root of tree */
        if (new_node != NULL)
            update_root(tree, new_node);
    }
}


/* Removes the key from the tree if it contains one */
void remove_key(Tree_2_3 tree, TreeKey value)
{
    log_trace("%s", __func__);

    /* Empty tree */
    if (tree == NULL)
    {
        log_error("Try work with nullable node");
        exit(EXIT_FAILURE);
    }

    /* TODO: maybe implement error/exception ? */
    if (tree_is_empty(tree))
        return;

    /* Key not in tree */
    if (!search_key(tree, value))
        return;


    struct _node *tmp = NULL;
    struct _node *deleted = delete_value(tree->root, value);

    tree->elements--;

    /* Need to update the root of tree
     * because he had only one child left */
    if (deleted != NULL)
    {
        if (tree->root->type == INNER)
        {
            tmp = tree->root->first;
            free(tree->root);
            tree->root = tmp;
        }
        else /* Make tree empty */
        {
            tree_make_empty(tree);
        }
    }
}


/* Return addres leaf with value or null if value not found
 * Wrapper function for finding the key. It is necessary that the user
 * does not call the root of the tree, but simply passes the tree itself */
struct _node * search_key(Tree_2_3 tree, TreeKey value)
{
    log_trace("%s", __func__);

    return search_value(tree->root, value);
}


/* Prints the node structure depending on its type
   you need to specify a function to print the value of the key */
void node_print(struct _node *node, func_print_key print_key)
{
    log_trace("%s", __func__);

    if (node == NULL)
    {
        puts("Node is empty!");
        return;
    }

    switch(node->type)
    {
        case LEAF:
                    puts("Node is leaf");
                    printf("Adr: %p\n",(void*)node);
                    printf("Val: ");
                    print_key(node->key);
                    putchar('\n');
                    break;

        case INNER:
                    puts("Node is inner");
                    printf("Adr: %p\n", (void*)node);
                    printf("First adr: %p\n", (void*)node->first);
                    printf("Second adr: %p\n", (void*)node->second);
                    printf("Third adr: %p\n", (void*)node->third);

                    printf("Min second: ");
                    print_key(node->second_min);
                    putchar('\n');

                    if (node->third)
                    {
                        printf("Min third: ");
                        print_key(node->third_min);
                        putchar('\n');
                    }
                    break;

        case EMPTY:
                    puts("Node is empty");
                    printf("Adr: %p\n", (void*)node);
                    break;

        default:
                    printf("Error! Incorrect node!");
                    break;
    }
}


/* Print tree. Need to pass a custom function to print the key */
void tree_print(Tree_2_3 tree, func_print_key print_key)
{
    log_trace("%s", __func__);

    int num_element = 0;

    print_tree_elements_in_order(tree->root, &num_element, print_key);
}


bool tree_is_empty(Tree_2_3 tree)
{
    log_trace("%s", __func__);

    return tree_count_elements(tree) == 0;
}


/* Getter for the root of tree */
struct _node * tree_get_root(Tree_2_3 tree)
{
    log_trace("%s", __func__);

    return tree->root;
}


/* Count of elements(leafs) in tree */
int tree_count_elements(Tree_2_3 tree)
{
    log_trace("%s", __func__);

    return tree->elements;
}


/* Return height of tree */
int tree_height(Tree_2_3 tree)
{
    log_trace("%s", __func__);

    if (tree == NULL || tree_is_empty(tree))
        return 0;

    int height = 0;
    struct _node *current = tree->root;


    while (current->type != LEAF)
    {
        height++;
        current = current->first;
    }

    return height;
}


/* Returns key of node */
TreeKey node_get_key(Node_2_3 node)
{
    log_trace("%s", __func__);

    if (node->type == LEAF)
        return node->key;
    return NULL;
}


/* Returns the minimum key in the tree */
TreeKey tree_get_min(Tree_2_3 tree)
{
    log_trace("%s", __func__);

    if (tree == NULL)
    {
        log_error("Can't find min value in empty tree!");
        exit(EXIT_FAILURE);
    }

    if (tree_is_empty(tree))
    {
        log_error("Can't find max value in empty tree!");
        exit(EXIT_FAILURE);
    }

    return get_min(tree->root);
}


/* Returns the maximum key in the tree */
TreeKey tree_get_max(Tree_2_3 tree)
{
    log_trace("%s", __func__);

    struct _node *current = tree->root;


    if (tree == NULL)
    {
        log_error("Can't find max value in non exist tree!");
        exit(EXIT_FAILURE);
    }

    if (tree_is_empty(tree))
    {
        log_error("Can't find max value in empty tree!");
        exit(EXIT_FAILURE);
    }


    while (current->type != LEAF)
    {
        if (child_cnt(current) == 3)
            current = current->third;
        else
        if (child_cnt(current) == 2)
            current = current->second;
        else
        {
            log_error("Inner node with one child not valid!");
            exit(EXIT_FAILURE);
        }
    }

    return current->key;
}


/* Release all nodes in tree and make tree is empy */
void tree_make_empty(Tree_2_3 tree)
{
    log_trace("%s", __func__);
    
    tree_free(tree->root);

    tree->root = NULL;
    tree->elements = 0;
}


/* Releases the memory and resources allocated for the tree */
void tree_destroy(Tree_2_3 tree)
{
    log_trace("%s", __func__);

    tree_free(tree->root);
    free(tree);
}
