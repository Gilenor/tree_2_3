/* A program for testing the correct operation of 2-3 trees */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>
#include "tree_2_3.h"

#define SIZE_ARR(arr)   (sizeof(arr)/sizeof(*arr))


typedef void * (func_get_arr_el_adr) (void *, unsigned);


/* ========================================================================= */


int cmp_double(tree_key r, tree_key l)
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


void * get_adr_double(void *arr, unsigned index)
{
    return (double*)arr + index;
}


/* ========================================================================= */


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


/* for some reason the function strcmp returns
   not '1' and '-1' but '> 0' and '< 0' and it is sad */
int cmp_string(tree_key r, tree_key l)
{
    int res = strcmp((char*)r, (char*)l);

    if (res < 0)
        res = LESS;
    else
    if (res > 0)
        res = GREATER;

    return res;
}


void * get_adr_string(void *arr, unsigned index)
{
    return ((char**)arr)[index];
}


/* ========================================================================= */


double rand_between(double low, double high)
{
    int tmp = rand();

    /* in case the user is inattentive */
    if (low > high)
    {
        double tmp1 = low;
        low = high;
        high = tmp1;
    }
    return (tmp % (int)(high-low) + low) + ((double)tmp / (double)RAND_MAX);
}

void work_with_tree(Tree_2_3 tree, void *arr, unsigned len_arr,
                    func_print_key print_key, func_get_arr_el_adr get_addr_el)
{
    for (unsigned i = 0; i < len_arr; i++)
    {
        if (!search_key(tree_get_root(tree), get_addr_el(arr, i)))
            insert_key(tree, get_addr_el(arr, i));

        //print_tree(tree, print_key);
        //putchar('\n');
    }

    //putchar('\n');
    print_tree(tree, print_key);

    putchar('\n');
    printf("Height of tree: %d\n", tree_height(tree));
    printf("Elements in tree: %d\n", tree_count_elements(tree));
    putchar('\n');

    printf("Uniq elements in tree: %d\n", len_arr-(len_arr-tree_count_elements(tree)));
    printf("Duplicated elements in array: %d\n", len_arr-tree_count_elements(tree));
    putchar('\n');


    while (!tree_is_empty(tree))
    {
        tree_key deleted = tree_get_min(tree);

        if (search_key(tree_get_root(tree), deleted))
        {
            printf("Delete value ");
            print_key(deleted);
            printf(" from tree\n");
            remove_key(tree, deleted);
            //print_tree(tree, print_key);
        }
        else
        {
            printf("\tElement ");
            print_key(deleted);
            printf(" not in tree\n");
        }
    }
    puts("Tree is empty!");
}




int main(int argc, char **argv)
{
    struct timespec start2, end2;

    double nums[20];

    char *singers[] = { "Freddie Mercury", "Mick Jagger",  "Robert Plant",
                        "John Lennon",     "Jim Morrison", "Ozzy Osbourne",
                        "Elvis Presley",   "Axl Rose",     "David Bowie"};

	Tree_2_3 tree_string = NULL;
    Tree_2_3 tree_double = NULL;

    srand(time(NULL));
    clock_gettime(CLOCK_REALTIME, &start2);

    /* feel array nums with random values */
    for (size_t i = 0; i < SIZE_ARR(nums); i++)
        nums[i] = rand_between(-100, 100);

    tree_string = new_tree(cmp_string, copy_string, (func_free_key)free);
    tree_double = new_tree(cmp_double, copy_double, (func_free_key)free);

    work_with_tree(tree_string, singers, SIZE_ARR(singers), print_string, get_adr_string);
    putchar('\n');
    work_with_tree(tree_double, nums, SIZE_ARR(nums), print_double, get_adr_double);

	clock_gettime(CLOCK_REALTIME, &end2);

	putchar('\n');
	fprintf(stderr, "Time to work programm %f(clock_gettime)\n", (end2.tv_sec - start2.tv_sec) + 1e-9*(end2.tv_nsec - start2.tv_nsec));

	tree_delete(tree_string);
    tree_delete(tree_double);

    return 0;
}
