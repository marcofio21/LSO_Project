#include "list_library.h"

/*Crea una lista vuota, quindi inizializza la testa */
head_list * create_list (){
    head_list *ret = malloc(sizeof(head_list));

    ret->top_list = ret->reading_point = ret->bottom_list = ret->list = NULL;
    ret->num_node = 0;

    return(ret);
}

/*inserisce un nodo all'interno della lista*/
head_list * insert_node (head_list *head, void *data, FCREATENODE  fcreatenode, FINSERTNODE *finsertnode){
    if(head && data) {
        void *new_node = fcreatenode(data);
        head = finsertnode(head, new_node);
    }

    return(head);
}

void * search_list (head_list *head, void *key, FFINDNODE *ffindnode){
    void *ret =NULL;
    if(head) {
        ret = ffindnode(head, key);
    }
    return(ret);
}

head_list * delete_all_node (head_list *head, FGETNODEREADINGPOINT fgetnodereadingpoint){
    if(head){
        int end_list = 0;
        void *node_to_del = NULL;
        while(end_list == 0) {
            node_to_del = fgetnodereadingpoint(head,&end_list);   // ottengo il nodo da cancellare, e la funzione fgetnodereadingpoint, passerà automaticamente al prossimo, oppure all'inizio della lista; in questo caso NULL
            free(node_to_del);
            node_to_del = NULL;
        }
        free(head);
        head = NULL;
    }

    return(head);
}

void * read_sequential_node (head_list *head, FGETNODEREADINGPOINT fgetnodereadingpoint){
    int end_list = 0;
    void *ret = NULL;

    if(head) {
        fgetnodereadingpoint(head, &end_list);
        if (end_list != 0) { ret = NULL; }
    }
    return(ret);
}

head_list * modify_node(head_list *head, void *key, void *new_value, FFINDNODE ffindnode, FMODIFYNODE fmodifynode){
    void *node = NULL;
    if(head){
        node = ffindnode(head,key);
        fmodifynode(node,new_value);
    }

    return(head);
}

