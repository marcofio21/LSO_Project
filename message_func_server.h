#ifndef LSO_PROJECT_SIGNAL_SERVER_H
#define LSO_PROJECT_SIGNAL_SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

void            no_breaking_exec_err        (int code);
void            breaking_exec_err           (int code);

void            print_errno                 (int errno_code);

void            ok_conn();
void            retry_conn(int attempt_number);
void            bad_conn();

void            close_err_buf();

#endif //LSO_PROJECT_SIGNAL_SERVER_H
