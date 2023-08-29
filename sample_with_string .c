/* A program for testing the correct operation of 2-3 trees with key type string*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>
#include "tree_2_3.h"

#define SIZE_ARR(arr)   (sizeof(arr)/sizeof(*arr))


/* ========================================================================= */


/* For some reason the function strcmp returns
   not '1' and '-1' but '> 0' and '< 0' and it is sad */
int compare_string(tree_key r, tree_key l)
{
    int res = strcmp((char*)r, (char*)l);

    if (res < 0)
        res = LESS;
    else
    if (res > 0)
        res = GREATER;

    return res;
}


tree_key copy_string(tree_key key)
{
    size_t len = (strlen((char*)key) + 1);
    char *tmp = malloc(sizeof(char) * len);

    strcpy(tmp, (char*)key);

    return (tree_key)tmp;
}


void print_string(tree_key value)
{
    printf("%s", (char*)value);
}


/* ========================================================================= */



int main(int argc, char **argv)
{
    Tree_2_3 tree = NULL;
    char *singers[] = { "Freddie Mercury", "Mick Jagger",  "Robert Plant",
                        "John Lennon",     "Jim Morrison", "Ozzy Osbourne",
                        "Elvis Presley",   "Axl Rose",     "David Bowie"};


    tree = new_tree(compare_string, copy_string, (func_free_key)free);

    /* Feel tree with values from array */
    for (size_t i = 0; i < SIZE_ARR(singers); i++)
    {
        if (!search_key(tree, singers[i]))
        {
            insert_key(tree, singers[i]);
            printf("\t Insert element: < %s >\n", singers[i]);
        }

        print_tree(tree, print_string);
        putchar('\n');
    }

    putchar('\n');
    printf("Height of tree: %d\n", tree_height(tree));
    printf("Elements in tree: %d\n", tree_count_elements(tree));
    putchar('\n');

    printf("Uniq elements in array: %zd\n", SIZE_ARR(singers) - (SIZE_ARR(singers) - tree_count_elements(tree)));
    printf("Duplicated elements in array: %zd\n", SIZE_ARR(singers) - tree_count_elements(tree));
    putchar('\n');


    while (!tree_is_empty(tree))
    {
        tree_key deleted = tree_get_min(tree);

        if (search_key(tree, deleted))
        {
            printf("Delete value < %s > from tree\n", (char*)deleted);
            remove_key(tree, deleted);
        }
        else
        {
            printf("\tElement < %s > not int tree\n", (char*)deleted);
        }
    }
    puts("Tree is empty!");

    tree_delete(tree);

    return 0;
}
