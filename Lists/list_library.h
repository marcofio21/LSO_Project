#ifndef LSO_PROJECT_LIST_SERVER_ADDR_H
#define LSO_PROJECT_LIST_SERVER_ADDR_H

#include <stdlib.h>
#include <unistd.h>

typedef struct head_generic_list head_list;
typedef struct node_generic_list node_list;

struct head_generic_list{
    node_list               *top_list;
    node_list               *bottom_list;
    node_list               *reading_point;

    int                     num_node;
};

struct node_generic_list {
    void                    *value;
    node_list               *next;
};

typedef int                     (FCOMPVALUE)            (void *node1, void *node2);

head_list *             create_list             ();
head_list *             insert_node             (head_list *head, void *data);
void *                  search_node             (head_list *head, node_list *node_to_find, FCOMPVALUE fcompvalue);
head_list *             delete_all_node         (head_list *head);
void *                  read_sequential_node    (head_list *head, int * f_end_list);
int                     modify_node             (head_list *head, node_list *node_to_modify, void *new_value, FCOMPVALUE fcompvalue);
node_list *             get_head_list           (head_list *list);

#endif //LSO_PROJECT_LIST_SERVER_ADDR_H
