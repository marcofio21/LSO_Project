#include <malloc.h>
#include "message_func_server.h"

int size_buf = 128;
char *buf = NULL;

char *pool_be_err[] = {
        "Wrong number of args",             //0
        "Invalid PORT",                     //1
        "ERR_OPEN_FILE",                    //2
        "ERR_INT_FIRST_CONN",               //3
        "ERR_CREATE_SOCKET",                //4
        "ERR_CONV_ADDR_ATON",               //5
        "ERR_SOCKET",                       //6
        "ERR_LISTEN",                       //7
        "ERR_NO_SUCH_SERVERS_LIST",         //8

        NULL
};

char *pool_err[] = {
        "BAD_CLIENT_CONNECTION",
        "ERR_CONF_FILE_NOT_RIGHT_FORMAT",
        "ERR_READ_SERVER_COMM",
        NULL
};
void            no_breaking_exec_err        (int code){
    sprintf(buf,"%s\n\n",pool_err[code]);
    write(2,buf, strlen(buf));
    bzero(buf,sizeof(*buf));
}
void            breaking_exec_err           (int code){
    sprintf(buf, "%s\n", pool_be_err[code]);
    write(2, buf, strlen(buf));
    free(buf);
    exit(-1);
}

void            print_errno                 (int errno_code){
    sprintf(buf,"ERRNO : %d\n", errno_code);
    write(2,buf, strlen(buf));
    bzero(buf,sizeof(*buf));
}

void            ok_conn(){
    sprintf(buf, "\n\nConnessione Stabilita!\n\n");
    write(0,buf,strlen(buf));
    bzero(buf,sizeof(*buf));
}

void            retry_conn(int attempt_number){
    sprintf(buf,"Tentativo %d di Connessione con gli altri server...\n",attempt_number);
    write(0,buf,strlen(buf));
    bzero(buf,sizeof(*buf));
}

void            bad_conn(){
    sprintf(buf, "\n\nImpossibile stabile connessione con gli altri server.\n");
    write(0,buf,strlen(buf));
    bzero(buf,sizeof(*buf));
}