#ifndef LSO_PROJECT_LIST_SERVER_ADDR_H
#define LSO_PROJECT_LIST_SERVER_ADDR_H

#include <stdlib.h>
#include <unistd.h>


typedef struct head_list_generic head_list;

struct head_list_generic{
    void                    *list;

    void                    *top_list;
    void                    *bottom_list;
    void                    *reading_point;

    int                     num_node;
};


typedef void*                   (FCREATENODE)           (void       *data);
typedef head_list*              (FINSERTNODE)           (head_list  *head,      void  *node);
typedef void*                   (FFINDNODE)             (head_list  *head,      void  *);
typedef void                    (FMODIFYNODE)           (void       *node,      void *new_value);
typedef void*                   (FGETNODEREADINGPOINT)  (head_list  *head,      int   *f_end_list);

head_list *             create_list             ();
head_list *             insert_node             (head_list *head, void *data, FCREATENODE  fcreatenode, FINSERTNODE *finsertnode);
void *                  search_list             (head_list *head, void *key, FFINDNODE *ffindnode);
head_list *             delete_all_node         (head_list *head, FGETNODEREADINGPOINT fgetnodereadingpoint);
void *                  read_sequential_node    (head_list *head, FGETNODEREADINGPOINT fgetnodereadingpoint);
head_list *             modify_node             (head_list *head, void *key, void *new_value, FFINDNODE ffindnode, FMODIFYNODE fmodifynode);


#endif //LSO_PROJECT_LIST_SERVER_ADDR_H
