#include "list_addr_functions.h"

int comp_addr(void *node1, void *node2){
    int ret = -1;
    //MISSING CODE :D
    return(ret);
}

void * create_new_node(server_addr *addr, int status, int position){
    check_servers_node *ret = malloc(sizeof(check_servers_node));

    ret->server = addr;
    ret->status = 1;
    ret->position = position;

    return (ret);
}