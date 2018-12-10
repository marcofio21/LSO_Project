#ifndef LSO_PROJECT_LIST_ADDR_FUNCTIONS_H
#define LSO_PROJECT_LIST_ADDR_FUNCTIONS_H

#include <string.h>
#include "list_library.h"

typedef struct node_server_address node_server_addr;
/*typedef struct value_addr value_addr;*/

struct node_server_address{
    char                    *addr;
    int                     port;
    node_server_addr        *next;
};

/*
struct value_addr{
    int         port;
    char        *addr;
};
 */

void *          create_addr_server_node     (void *data);

head_list *     insert_addr_server_node     (head_list *head, void *node);

void *          search_addr_server_node     (head_list *head, void * key);

void            modify_addr_server_node     (void *node, void *new_value);

void *          get_addr_server_node        (head_list *head, int *f_end_list);

#endif //LSO_PROJECT_LIST_ADDR_FUNCTIONS_H
