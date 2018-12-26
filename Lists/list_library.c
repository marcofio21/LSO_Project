#include "list_library.h"

/*Crea una lista vuota, quindi inizializza la testa */
head_list * create_list (){
    head_list *ret = malloc(sizeof(head_list));

    ret->top_list = ret->reading_point = ret->bottom_list = NULL;
    ret->num_node = 0;

    return(ret);
}


/*crea ed inserisce un nodo all'interno della lista*/
head_list * insert_node (head_list *head, void *data){
    if(head && data) {
        //crea il nuovo nodo
        node_list *new_node = malloc(sizeof(node_list));
        new_node->value = data;
        new_node->next  = NULL;

        //lo inserisce nella lista
        if(head->top_list && head->bottom_list) {
            head->bottom_list->next     = new_node;
            head->bottom_list           = head->bottom_list->next;
        }else {
            head->top_list = head->bottom_list = new_node;
        }
        head->num_node += 1;

    }
    return(head);
}

void * search_node(head_list *head, node_list *node_to_find, FCOMPVALUE fcompvalue){
    node_list *p        = NULL;
    node_list *ret      = NULL;
    if(head && node_to_find){
        p = head->top_list;
        while(p && !ret){
            if(fcompvalue(p->value,node_to_find->value) == 0){
                ret = p;
            }else{
                p = p->next;
            }
        }
    }
    return (ret);
}
/*
head_list * delete_all_node (head_list *head){
    if(head){
        int end_list = 0;
        void *node_to_del = NULL;
        while(end_list == 0) {
            node_to_del = read_sequential_node(head,&end_list);   // ottengo il nodo da cancellare, e la funzione fgetnodereadingpoint, passerà automaticamente al prossimo, oppure all'inizio della lista; in questo caso NULL
            free(node_to_del);
            node_to_del = NULL;
        }
        free(head);
        head = NULL;
    }

    return(head);
}
 */
 /*
void * read_sequential_node (head_list *head, int *f_end_list){
    void *ret = NULL;
    if(head && f_end_list){
        if(!head->reading_point){
            if(head->top_list) {
                ret = head->top_list->value;
                head->reading_point = head->top_list->next;
                if (!head->reading_point) {
                    *f_end_list = 1;
                }
            }else{
                *f_end_list = 1;
            }
        }else {
            ret = head->reading_point->value;
            head->reading_point = head->reading_point->next;
            if (!head->reading_point) {
                head->reading_point = NULL;
                *f_end_list = 1;
            }
        }
    }

    return(ret);
}
*/
int modify_node(head_list *head, node_list *node_to_modify, void *new_value, FCOMPVALUE fcompvalue) {
    if(head && node_to_modify && new_value) {
        void *node = NULL;
        node = search_node(head, node_to_modify, fcompvalue);
        if (node) {
            return (1);
        } else {
            return (0);
        }
    }

    return(-1);
}

node_list * get_head_list(head_list *list){
    node_list *ret = NULL;
    if(list){
        ret = list->top_list;
        list->top_list = list->top_list->next;
    }
    return(ret);
}