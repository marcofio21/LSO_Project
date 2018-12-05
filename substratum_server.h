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
#include <pthread.h>

typedef struct returned_value_addr value_addr;

struct returned_value_addr{
    int         port;
    char        *addr;
};

//Calcola il valore di a elevato al valore intero di exp
int                 pow_int                             (int a, int exp);
//Controlla che la stringa input_string sia un intero che è nel range tra a e b, con questi compresi; RESTITUISCE il valore se corretto.
int                 checked_p_range_input               (char *input_string, int a, int b);
//Controlla che il formato delle stringhe date in input, rispetti il formato richiesto dalle specifiche per il file di configurazione
value_addr *        check_dot_addr                      (char *input, int length);
//crea un server e si mette in ascolto di 1 o piu client
int                 create_socket                       (int port, char *ip);
int                 comm_thread                         (value_addr *addr_server);
void                *commission_comm_server             (void *value);

#endif //LSO_PROJECT_SUBSTRATUM_SERVER_H
