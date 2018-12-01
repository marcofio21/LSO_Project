#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "substratum_server.h"

int main(int argc, char *argv[]) {
    int         fd                          = -1;
    int         check                       = 0;
    ssize_t     num_byte                    = -1;

    size_t      buf_dim                     = 350;
    int         err_buf_dim                 = 128;

    char    *err_buf    = malloc(err_buf_dim * sizeof(char));
    char    *buf        = malloc(buf_dim * sizeof(char));

    if(argc < 2){
        sprintf(err_buf, "Few Args\n");
        write(2,err_buf,strlen(err_buf));
        exit(-1);
    }
    if(argc > 2){
        sprintf(err_buf, "Too Many Args\n");
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

    do{
        num_byte = read(fd,buf,buf_dim);
        check = check_dot_addr(buf,(int)num_byte);
        if(check!=0){exit(-1);}


    }while(num_byte > 0);
}