#include <stdio.h>

#include "xmalloc.h"
#include "list.h"
#include "mergesort.h"

char *program_name = "list_mergesort_learn.c";

struct list {
    struct list_head node;

    int val;
};

int compare(const struct list_head *a,
            const struct list_head *b)
{
    struct list *la = container_of(a, struct list, node);
    struct list *lb = container_of(b, struct list, node);

    return la->val > lb->val;
}

int main(void)
{
    LIST_HEAD(sort_list);

    struct list *list_ptr = xnew(struct list, 1);
    list_ptr->val = 2;
    list_add_tail(&list_ptr->node, &sort_list);
    list_ptr = xnew(struct list, 1);
    list_ptr->val = 5;
    list_add_tail(&list_ptr->node, &sort_list);
    list_ptr = xnew(struct list, 1);
    list_ptr->val = 3;
    list_add_tail(&list_ptr->node, &sort_list);
    list_ptr = xnew(struct list, 1);
    list_ptr->val = 1;
    list_add_tail(&list_ptr->node, &sort_list);
    list_ptr = xnew(struct list, 1);
    list_ptr->val = 6;
    list_add_tail(&list_ptr->node, &sort_list);

    list_mergesort(&sort_list, compare);

    list_for_each_entry(list_ptr, &sort_list, node) {
        printf("%d ", list_ptr->val);
    }
    printf("\n");


    /* TODO free */

    return 0;
}
