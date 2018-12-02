#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "substratum_server.h"



int main(int argc, char *argv[]) {
    int         i                           = 0;
    int         offset                      = 0;

    int         fd                          = -1;
    int         check                       = 0;
    int         num_byte                    = -1;

    size_t      buf_dim                     = 350;
    int         err_buf_dim                 = 128;
    size_t      addr_lenght                 = 21;

    char    *err_buf    = malloc(err_buf_dim * sizeof(char));
    char    *buf        = malloc(buf_dim * sizeof(char));
    char    *addr       = malloc(addr_lenght * sizeof(char));


    if(argc != 3){
        sprintf(err_buf, "Wrong number of args\n");
        write(2,err_buf,strlen(err_buf));
        exit(-1);
    }
    if((checked_p_range_input(argv[2],0,9999)<0)){
        sprintf(err_buf, "Invalid PORT\n");
        write(2,err_buf,strlen(err_buf));
        exit(-1);
    }

    fd = open(argv[1], O_RDONLY);
    if(fd<0){sprintf(err_buf,"ERR_OPEN_FILE\n\n");write(2,err_buf,strlen(err_buf));exit(-1);}


    while(read(fd, buf+i, 1) == 1) {
        if (buf[i] == '\n' || buf[i] == 0x0) {
            num_byte = i - offset;
            strncpy(addr,buf+offset,(size_t)num_byte);
            check = check_dot_addr(addr, num_byte);
            if (check != 0) { exit(-1); }
            offset = i + 1;
            //Qua va creato il thread con la connessione al server.
        }
        ++i;
    }
}