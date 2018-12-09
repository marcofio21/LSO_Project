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

#include "list_library.h"
#include "list_addr_functions.h"


//Calcola il valore di a elevato al valore intero di exp.
int                 pow_int                             (int a, int exp);

//Controlla che la stringa input_string sia un intero che è nel range tra a e b, con questi compresi; RESTITUISCE il valore se corretto.
int                 checked_p_range_input               (char *input_string, int a, int b);

//Controlla che il formato delle stringhe date in input, rispetti il formato richiesto dalle specifiche per il file di configurazione.
value_addr *        check_dot_addr                      (char *input, int length);

//crea un server e si mette in ascolto di 1 o piu client
int                 create_socket                       (int port, char *ip);
int                 comm_thread                         (value_addr *addr_server);
/*
void *              commission_comm_server              (void *server_addr_in);
*/

//Restituisce lo stato della connessione con gli altri server
int                 get_num_conn_o_server               ();

//Funzione richiamata dal Thread che si occupa del controllo della disponibilità degli altri server.
void *              check_conn_o_server                 (void *server_addr_in);

//Funzione che controlla lo stato della connessione con un server.
int                 check_conn_o_server_thread(value_addr *addr_server_to_check);

#endif //LSO_PROJECT_SUBSTRATUM_SERVER_H
