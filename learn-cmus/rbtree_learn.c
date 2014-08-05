#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"

struct rbtree_learn {
    struct rb_node rbtree_learn_node;
    int num;
};

struct rbtree_learn *
rbtree_learn_search(struct rb_root *root, int num)
{
    struct rb_node *node = root->rb_node;

    while (node) {
        struct rbtree_learn *data = 
            container_of(node, struct rbtree_learn, rbtree_learn_node);

        if (num < data->num)
            node = node->rb_left;
        else if (num > data->num)
            node = node->rb_right;
        else
            return data;
    }

    return NULL;
}

int rbtree_learn_insert(struct rb_root *root, struct rbtree_learn *data)
{
    struct rb_node **tmp = &(root->rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
        struct rbtree_learn *this = 
            container_of(*tmp, struct rbtree_learn, rbtree_learn_node);

        parent = *tmp;
        if (data->num < this->num)
            tmp = &((*tmp)->rb_left);
        else if (data->num > this->num)
            tmp = &((*tmp)->rb_right);
        else 
            return -1;
    }

    /* Add new node and rebalance tree. */
    rb_link_node(&data->rbtree_learn_node, parent, tmp);
    rb_insert_color(&data->rbtree_learn_node, root);

    return 0;
}

void rbtree_learn_delete(struct rb_root *root, int num)
{
    struct rbtree_learn *data = rbtree_learn_search(root, num);
    if (!data) { 
        fprintf(stderr, "Not found %d.\n", num);
        return;
    }

    rb_erase(&data->rbtree_learn_node, root);
    free(data);
}

void print_rbtree(struct rb_root *tree)
{
    struct rb_node *node;
    struct rbtree_learn *type;

    /*
    for (node = rb_first(tree); node; node = rb_next(node))
        printf("%d ", 
        rb_entry(node, struct rbtree_learn, rbtree_learn_node)->num);
    */

    rb_for_each_entry(type, node, tree, rbtree_learn_node)
        printf("%d ", type->num);

    printf("\n");
}

int main(int argc, char *argv[])
{
    struct rb_root rbtree_learn_tree = RB_ROOT;
    struct rbtree_learn *tmp;

    /*
    int i, ret, num;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s num\n", argv[0]);
        exit(-1);
    }

    num = atoi(argv[1]);

    printf("Please enter %d integers:\n", num);
    for (i = 0; i < num; i++) {
        tmp = malloc(sizeof(struct rbtree_learn));
        if (!tmp)
            perror("Allocate dynamic memory");

        scanf("%d", &tmp->num);

        ret = rbtree_learn_insert(&rbtree_learn_tree, tmp);
        if (ret < 0) {
            fprintf(stderr, "The %d already exists.\n", tmp->num);
            free(tmp);
        }
    }
    */

    tmp = malloc(sizeof(struct rbtree_learn));
    tmp->num = 8;
    rbtree_learn_insert(&rbtree_learn_tree, tmp);

    tmp = malloc(sizeof(struct rbtree_learn));
    tmp->num = 87;
    rbtree_learn_insert(&rbtree_learn_tree, tmp);

    tmp = malloc(sizeof(struct rbtree_learn));
    tmp->num = 7;
    rbtree_learn_insert(&rbtree_learn_tree, tmp);

    tmp = malloc(sizeof(struct rbtree_learn));
    tmp->num = 21;
    rbtree_learn_insert(&rbtree_learn_tree, tmp);

    tmp = malloc(sizeof(struct rbtree_learn));
    tmp->num = 13;
    rbtree_learn_insert(&rbtree_learn_tree, tmp);

    tmp = malloc(sizeof(struct rbtree_learn));
    tmp->num = 33;
    rbtree_learn_insert(&rbtree_learn_tree, tmp);

    tmp = malloc(sizeof(struct rbtree_learn));
    tmp->num = 63;
    rbtree_learn_insert(&rbtree_learn_tree, tmp);

    printf("\norigin \n");
    print_rbtree(&rbtree_learn_tree);

    rbtree_learn_delete(&rbtree_learn_tree, 21);

    printf("\ndelete 21\n");
    print_rbtree(&rbtree_learn_tree);

    /* free */
    struct rb_node *node1, *node2;
    rb_for_each_safe(node1, node2, &rbtree_learn_tree) {
        rb_erase(node1, &rbtree_learn_tree);
        free(container_of(node1, struct rbtree_learn, rbtree_learn_node));
    }

    return 0;
}

