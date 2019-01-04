#include "list_addr_functions.h"

int comp_addr(void *node1, void *node2){
    int ret = -1;

    check_servers_node *t1 = node1;
    check_servers_node *t2 = node2;

    if((strcmp(t1->server->addr,t2->server->addr)) == 0){
        if(t1->server->port == t2->server->port){
            ret = 0;
        }
    }

    return(ret);
}

void *create_new_node(server_addr *addr) {
    check_servers_node *ret = malloc(sizeof(check_servers_node));

    ret->server = addr;
    ret->status = 1;

    return (ret);
}