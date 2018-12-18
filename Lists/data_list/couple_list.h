#ifndef LSO_PROJECT_COUPLE_LIST_H
#define LSO_PROJECT_COUPLE_LIST_H

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

typedef struct effective_data_server mem_data;

struct effective_data_server{
    char *key;
    char *value;
};

int         comp_couples            (void *node1, void *node2);
void *      create_new_couples_node (char *key, char *value);

#endif //LSO_PROJECT_COUPLE_LIST_H
