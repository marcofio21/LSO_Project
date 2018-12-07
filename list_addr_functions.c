#include "list_addr_functions.h"

void * create_addr_server_node (void *data){
    void * ret = NULL;
    node_server_addr *new_node = NULL;

    if(data) {
        new_node = malloc(sizeof(node_server_addr));
        new_node->addr = ((node_server_addr *) data)->addr;
        new_node->port = ((node_server_addr *) data)->port;
        new_node->next = NULL;

        ret = new_node;
    }
    return(ret);
}

head_list * insert_addr_server_node (head_list *head, void *node){
    if(head && node){
        ((node_server_addr *)(head->bottom_list))->next = node;
        head->bottom_list = ((node_server_addr *)(head->bottom_list))->next;
    }

    return (head);
}

void * search_addr_server_node (head_list *head, void * key){
    void *ret = NULL;
    if(head && key){
        node_server_addr *p = head->top_list;
        value_addr *conv_key = key;
        while(p && !ret){
            if((strcmp((conv_key->addr),p->addr)) == 0 && p->port == conv_key->port){
                ret = p;
            }else{
                p = p->next;
            }
        }
    }
    return (ret);
}

void modify_addr_server_node(void *node, void *new_value){
    if(node && new_value){
        node_server_addr *p = node;
        value_addr *conv_new_value = new_value;

        strcpy(p->addr,conv_new_value->addr);
        p->port = conv_new_value->port;
    }
}

void * get_addr_server_node(head_list *head, int *f_end_list){
    void *ret = NULL;
    if(head && f_end_list){
        if(!head->reading_point){
            ret = head->top_list;
            if(head->top_list) {
                head->reading_point = ((node_server_addr *) (head->top_list))->next;
                if (!head->reading_point) {
                    *f_end_list = 1;
                }
            }else{
                *f_end_list = 1;
            }
        }else {
            ret = head->reading_point;
            head->reading_point = ((node_server_addr *) (head->reading_point))->next;
            if (!head->reading_point) {
                *f_end_list = 1;
            }
        }
    }

    return(ret);
}