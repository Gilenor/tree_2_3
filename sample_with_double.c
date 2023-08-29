/* A program for testing the correct operation of 2-3 trees with key type double*/

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>
#include "tree_2_3.h"

#define SIZE_ARR(arr)   (sizeof(arr)/sizeof(*arr))


/* ========================================================================= */


int compare_double(tree_key r, tree_key l)
{
    if (*(double*)r > *(double*)l)
        return GREATER;

    if (*(double*)r < *(double*)l)
        return LESS;

    return EQUAL;
}


tree_key copy_double(tree_key key)
{
    tree_key tmp = malloc(sizeof(double));

    *(double*)tmp = *(double*)key;

    return tmp;
}


void print_double(tree_key value)
{
    printf("%f", *(double*)value);
}


/* ========================================================================= */


double rand_between(double low, double high)
{
    int tmp = rand();

    /* In case the user is inattentive */
    if (low > high)
    {
        double tmp1 = low;
        low = high;
        high = tmp1;
    }
    return (tmp % (int)(high-low) + low) + ((double)tmp / (double)RAND_MAX);
}


/* ========================================================================= */


int main(int argc, char **argv)
{
    double nums[20];
    Tree_2_3 tree = NULL;


    srand(time(NULL));

    /* Feel array nums with random values */
    for (size_t i = 0; i < SIZE_ARR(nums); i++)
        nums[i] = rand_between(-100, 100);

    tree = new_tree(compare_double, copy_double, (func_free_key)free);

    /* Feel tree with values from array */
    for (size_t i = 0; i < SIZE_ARR(nums); i++)
    {
        if (!search_key(tree, &nums[i]))
        {
            insert_key(tree, &nums[i]);
            printf("\t Insert element: < %f >\n", nums[i]);
        }

        print_tree(tree, print_double);
        putchar('\n');
    }

    putchar('\n');
    printf("Height of tree: %d\n", tree_height(tree));
    printf("Elements in tree: %d\n", tree_count_elements(tree));
    putchar('\n');

    printf("Uniq elements in array: %zd\n", SIZE_ARR(nums) - (SIZE_ARR(nums) - tree_count_elements(tree)));
    printf("Duplicated elements in array: %zd\n", SIZE_ARR(nums) - tree_count_elements(tree));
    putchar('\n');


    while (!tree_is_empty(tree))
    {
        tree_key deleted = tree_get_min(tree);

        if (search_key(tree, deleted))
        {
            printf("Delete value < %f > from tree\n", *(double*)deleted);
            remove_key(tree, deleted);
        }
        else
        {
            printf("\tElement < %f > not int tree\n", *(double*)deleted);
        }
    }
    puts("Tree is empty!");

    tree_delete(tree);

    return 0;
}
