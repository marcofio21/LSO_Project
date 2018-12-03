#ifndef LSO_PROJECT_LIST_SERVER_ADDR_H
#define LSO_PROJECT_LIST_SERVER_ADDR_H

#include "substratum_server.h"

typedef struct node_list_server node_server;
typedef struct list_server_head head_list_serv;

struct list_server_head{
    node_server             *top_list;
    node_server             *bottom_list;
    node_server             *reading_point;
};


struct node_list_server{
    node_server             *next;
    value_addr              *info_addr_server;
};

head_list_serv * insert(head_list_serv *head, value_addr *addr_server);
head_list_serv * delete_all_list(head_list_serv *head);
node_server *    read_sequential_node(head_list_serv *head);

#endif //LSO_PROJECT_LIST_SERVER_ADDR_H
