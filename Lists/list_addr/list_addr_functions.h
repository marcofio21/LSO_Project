#ifndef LSO_PROJECT_LIST_ADDR_FUNCTIONS_H
#define LSO_PROJECT_LIST_ADDR_FUNCTIONS_H

#include <string.h>
#include "../list_library.h"

typedef struct server_address server_addr;


struct server_address{
    char                    *addr;
    int                     port;
};

int comp_addr(void *node1, void *node2);

#endif //LSO_PROJECT_LIST_ADDR_FUNCTIONS_H
