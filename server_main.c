#include "substratum_server.h"

//Lista server per la comunicazione interna.

int                     socket_client_fd                = -1;
int                     inner_socket_fd                 = -1;
int                     *client_fd;

server_addr             *this_server_inner_addr         = NULL;
server_addr             *this_server_client_addr        = NULL;

int main(int argc, char *argv[]) {
    f_err               = 0;

    int         t       = 1;
    int         check   = 0;
    int         dim_buf = 20;
    ssize_t     readed  = 0;

    char        *buf    = malloc(dim_buf * sizeof(char));
    int         *err_t  = NULL;

    this_server_inner_addr      = malloc(sizeof(server_addr));
    this_server_client_addr     = malloc(sizeof(server_addr));

    server_addr *ret_addr = NULL;

    data_couples_list = create_list();
    data_couples_list->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(data_couples_list->mutex,NULL);

    if (argc != 3) {breaking_exec_err(0);}

    int t_port = (checked_p_range_input(argv[2], 0, 9999));
    if (t_port < 0) {breaking_exec_err(1);}

    this_server_inner_addr = create_list_other_server(argv[1]);
    inner_port = this_server_inner_addr->port;

    //ottenuto l'indirizzo a cui connettersi con gli altri server, questo sarà in comune anche con il client
    this_server_client_addr->addr       = this_server_inner_addr->addr;
    this_server_client_addr->port       = t_port;
    client_port = t_port;

    inner_socket_fd = create_socket(this_server_inner_addr->port,this_server_inner_addr->addr);
    comm_thread(&lister_from_other_server,&inner_socket_fd);
    if (inner_socket_fd < 0) {breaking_exec_err(4);}

    //socket in ascolto rispetto gli altri server.
    check = first_conn_interface();
    if(check!=1){breaking_exec_err(3);}


    socket_client_fd = create_socket(this_server_client_addr->port,this_server_client_addr->addr);
    if (socket_client_fd < 0) {breaking_exec_err(4);}

    while(check != 0) {
        client_fd = malloc(sizeof(int));
        *client_fd  = accept(socket_client_fd, NULL, NULL);

        t = conn_oth_server_control();
        if(t != 1){
            f_err = 1;
        }

        if(f_err == 0) {
            if (client_fd < 0) {
                no_breaking_exec_err(0);
            } else {
                readed = read(*client_fd, buf, 1);
                if (readed < 0) {
                    no_breaking_exec_err(3);
                } else {
                    if (buf[0] == 's') {
                        /*comando STORE */
                        comm_thread(&store, client_fd);
                    } else if (buf[0] == 'c') {
                        /*comand CORRUPT*/
                        comm_thread(&corrupt, client_fd);
                    } else if (buf[0] == 'S') {
                        /*comand SEARCH*/
                        comm_thread(&search, client_fd);
                    } else if (buf[0] == 'l') {
                        /*comand LIST*/
                        comm_thread(&list, client_fd);
                    }
                }
            }
        }else{
            no_breaking_exec_err(4);
        }
        client_fd = NULL;
    }
}