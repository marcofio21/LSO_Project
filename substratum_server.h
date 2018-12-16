#ifndef LSO_PROJECT_SUBSTRATUM_SERVER_H
#define LSO_PROJECT_SUBSTRATUM_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "Lists/list_library.h"
#include "Lists/list_addr/list_addr_functions.h"


head_list       *servers_check_list;

//Calcola il valore di a elevato al valore intero di exp.
int                 pow_int                             (int a, int exp);

//Controlla che la stringa input_string sia un intero che è nel range tra a e b, con questi compresi; RESTITUISCE il valore se corretto.
int                 checked_p_range_input               (char *input_string, int a, int b);

//Controlla che il formato delle stringhe date in input, rispetti il formato richiesto dalle specifiche per il file di configurazione.
server_addr *       check_dot_addr                      (char *input, int length);



//interfaccia che integra tutta la logica per il controllo dello stato degli altri server. restituirà 1 se sono online, oppure farà la EXIT direttamente.
int                 first_conn_interface();

int                 create_socket(int port, char *ip);

//crea un server e si mette in ascolto
int                 comm_thread(int *socked_fd);

//Funzione richiamata dal Thread che si occupa del controllo della disponibilità degli altri server.
void *              thread_oth_server_job(void *server_node);

//Funzione che controlla lo stato della connessione con un server.
pthread_t           thread_oth_server(check_servers_node *server_to_check);

int                 check_conn_to_other_server();

#endif //LSO_PROJECT_SUBSTRATUM_SERVER_H
