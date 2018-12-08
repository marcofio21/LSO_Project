#include "substratum_server.h"

//Lista server per la comunicazione interna.
head_list *list_server_address = NULL;

int     *list_data_from_server = NULL;  // per la search; quando viene eseguita, gli altri server devono comunicare
                                        // la loro coppia di chiave x e che valore ha associato y.

int main(int argc, char *argv[]) {
    int         i                           = 0;
    int         offset                      = 0;

    int         fd                          = -1;
    int         check                       = 0;
    int         num_byte                    = -1;

    size_t      buf_dim                     = 350;
    int         err_buf_dim                 = 128;
    size_t      addr_lenght                 = 21;

    char    *err_buf                        = malloc(err_buf_dim * sizeof(char));
    char    *buf                            = malloc(buf_dim * sizeof(char));
    char    *readed_addr                    = malloc(addr_lenght * sizeof(char));

    value_addr *ret_addr =  NULL;

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

    list_server_address = create_list();

    while(read(fd, buf+i, 1) == 1) {
        if (buf[i] == '\n' || buf[i] == 0x0) {
            num_byte = i - offset;
            strncpy(readed_addr,buf+offset,(size_t)num_byte);
            ret_addr = check_dot_addr(readed_addr, num_byte);

            if (!ret_addr) { exit(-1); }
            offset = i + 1;

            list_server_address = insert_node(list_server_address,ret_addr,&create_addr_server_node,&insert_addr_server_node);

            //Qua va creato il thread con la connessione al server.
            check = comm_thread(ret_addr);
            if(check!=0){
                sprintf(err_buf,"ERR_CREATE_THREAD\n");
                write(2,err_buf,strlen(err_buf));
                free(err_buf);
                err_buf = NULL;
                exit(-1);
            }

        }
        ++i;
    }
    sleep(1);
}