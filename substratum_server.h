#ifndef LSO_PROJECT_SUBSTRATUM_SERVER_H
#define LSO_PROJECT_SUBSTRATUM_SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
//Calcola il valore di a elevato al valore intero di exp
int                 pow_int                             (int a, int exp);
//Controlla che la stringa input_string sia un intero che è nel range tra a e b, con questi compresi; RESTITUISCE il valore se corretto.
int                 checked_p_range_input               (char *input_string, int a, int b);
//Controlla che il formato delle stringhe date in input, rispetti il formato richiesto dalle specifiche per il file di configurazione
int                 check_dot_addr                      (char *input, int length);
//crea un server e si mette in ascolto di 1 o piu client
void                create_Server                        (int port,char ip);

#endif //LSO_PROJECT_SUBSTRATUM_SERVER_H
