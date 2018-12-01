#ifndef LSO_PROJECT_SUBSTRATUM_SERVER_H
#define LSO_PROJECT_SUBSTRATUM_SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//non so cosa deve fare bulabula
int                 pow_int                             (int a, int exp);
int                 checked_p_range_input               (char *input_string, int a, int b);
int                 check_dot_addr                      (char *input, int length);

#endif //LSO_PROJECT_SUBSTRATUM_SERVER_H
