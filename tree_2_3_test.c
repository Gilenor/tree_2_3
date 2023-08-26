/* A program for testing the correct operation of 2-3 trees */

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>
#include "tree_2_3.h"

#define RANDOM
#define ELEMENTS    100



int main(int argc, char **argv)
{
    struct timespec start2, end2;

    clock_gettime(CLOCK_REALTIME, &start2);

#ifdef RANDOM
	value_t v[ELEMENTS];
	int elements = ELEMENTS;

    srand(time(NULL));
#else
    value_t v[] = { 2, 5, 7, 8, 12, 16, 19, 20, 12, 4, 7, 8 };
    int elements = sizeof(v) / sizeof(*v);
#endif // RANDOM

	int cnt_elements = 0;
	struct node *finded = NULL;
	TREE_2_3 tree = new_tree();




    for (int i = 0; i < elements; i++)
    {
#ifdef RANDOM
        v[i] = rand() % elements - rand() % elements;
#endif

        if (!search_value(tree, v[i]))
            insert_value(&tree, v[i]);
    }

    putchar('\n');
    cnt_elements = tree_count_elements(tree);
    print_tree(tree);

    putchar('\n');
    printf("Height of tree: %d\n", tree_height(tree));
	printf("Elements in tree: %d\n", cnt_elements);
	putchar('\n');

    printf("Uniq elements in tree: %d\n", elements-(elements-cnt_elements));
    printf("Duplicated elements in array: %d\n", elements-cnt_elements);
	printf("sizeof struct node: %zu\n", sizeof(struct node));


    while (!tree_is_empty(tree))
    {
        value_t deleted = tree_get_min(tree);

        if (deleted > 0)
            deleted = tree_get_max(tree);

        if (search_value(tree, deleted))
        {
            printf("Delete value %d from tree\n", deleted);
            remove_value(&tree, deleted);
            //print_tree(tree);
        }
        else
            printf("\tElement %d not in tree\n", deleted);
    }

	tree_delete(tree);

	clock_gettime(CLOCK_REALTIME, &end2);
	fprintf(stderr, "Time to work programm %f(clock_gettime)\n", (end2.tv_sec - start2.tv_sec) + 1e-9*(end2.tv_nsec - start2.tv_nsec));

	return 0;
}
