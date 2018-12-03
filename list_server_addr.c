#include <stdlib.h>
#include <unistd.h>
#include "list_server_addr.h"

head_list_serv * insert(head_list_serv *head, value_addr *addr_server){

    if(head){
        if(head->top_list && head->bottom_list){
            head->bottom_list->next = malloc(sizeof(node_server));
            head->bottom_list = head->bottom_list->next;
        }else{
            write(2,"ERR_LIST_SERVER\n",16);
            exit(-1);
        }
    }else{
        head = malloc(sizeof(head_list_serv));
        head->top_list = NULL;
        head->bottom_list = NULL;
        head->reading_point = NULL;

        head->top_list = malloc(sizeof(node_server));
        head->top_list->next = NULL;
        head->top_list->info_addr_server = addr_server;

        head->bottom_list = head->reading_point = head->top_list;    }

    return (head);
}

head_list_serv * delete_all_list1(head_list_serv *head){
    if(head){
        if(head->top_list){
            node_server *t;
            t = head->top_list;
            head->top_list = head->top_list->next;

            free(t);

            delete_all_list1(head);
        }
    }

    return(head);
}

head_list_serv * delete_all_list(head_list_serv *head){
    head = delete_all_list1(head);

    free(head);

    return (NULL);
}


node_server * read_sequential_node(head_list_serv *head){
    node_server * ret = NULL;
    if(head){
        if(head->reading_point){
            ret = head->reading_point;
            head->reading_point = head->reading_point->next;
        }else{
            head->reading_point = head->top_list;
        }
    }

    return(ret);
}