#ifndef LSO_PROJECT_LIST_ADDR_FUNCTIONS_H
#define LSO_PROJECT_LIST_ADDR_FUNCTIONS_H

#include <string.h>
#include <stdlib.h>

typedef     struct server_address           server_addr;
typedef     struct check_oth_serv_node      check_servers_node;

struct server_address{
    char                    *addr;
    int                     port;
};

struct check_oth_serv_node{
    server_addr             *server;
    int                     position;
    int                     status;
};

void * create_new_node(server_addr *addr, int status, int position);
int comp_addr(void *node1, void *node2);

#endif //LSO_PROJECT_LIST_ADDR_FUNCTIONS_H
