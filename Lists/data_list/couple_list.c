#include "couple_list.h"

int comp_couples(void *node1, void *node2){
    int ret = -1;

    if(node1 && node2) {

        mem_data *n1 = node1;
        mem_data *n2 = node2;

        ret = strcmp(n1->key,n2->key);
        /*f(ret != 0){
            ret = strcmp(n1->value, n2->value);
            if(ret != 0){
                ret = 2; //la chiave è la stessa, ma non il valore associato. Collisione.
            }
        }else{
                ret = 1; //la chiave differente.
        }*/

        if(ret == 0){
            ret = 0;
        }else{
            ret = 1;
        }
    }

    return(ret); // 0, nodi uguali; 1, chiave differente, OK;
}

void * create_new_couples_node(char *key, char *value){
    mem_data *ret = malloc(sizeof(mem_data));

    ret->key    = key;
    ret->value  = value;

    return (ret);
}