#include "substratum_server.h"

//Lista server per la comunicazione interna.
server_addr             *this_server_inner_addr         = NULL;
server_addr             *this_server_client_addr        = NULL;
int                     socket_client_fd                = -1;
int                     inner_socket_fd                 = -1;
int                     *client_fd;

int main(int argc, char *argv[]) {
    int         check   = 0;
    int         dim_buf = 20;
    ssize_t     readed  = 0;

    char        *buf    = malloc(dim_buf * sizeof(char));
    int         *err_t  = NULL;

    this_server_inner_addr      = malloc(sizeof(server_addr));
    this_server_client_addr     = malloc(sizeof(server_addr));

    server_addr *ret_addr = NULL;

    if (argc != 3) {breaking_exec_err(0);}

    int t_port = (checked_p_range_input(argv[2], 0, 9999));
    if (t_port < 0) {breaking_exec_err(1);}

    this_server_inner_addr = create_list_other_server(argv[1]);

    //ottenuto l'indirizzo a cui connettersi con gli altri server, questo sarà in comune anche con il client
    this_server_client_addr->addr       = this_server_inner_addr->addr;
    this_server_client_addr->port       = t_port;

    inner_socket_fd = create_socket(this_server_inner_addr->port,this_server_inner_addr->addr);
    comm_thread(&lister_from_other_server,&inner_socket_fd);
    if (inner_socket_fd < 0) {breaking_exec_err(4);}

    //socket in ascolto rispetto gli altri server.
    check = first_conn_interface();
    if(check!=1){breaking_exec_err(3);}

    data_couples_list = create_list();

    socket_client_fd = create_socket(this_server_client_addr->port,this_server_client_addr->addr);
    if (socket_client_fd < 0) {breaking_exec_err(4);}
    client_fd = malloc(sizeof(int));

    while(check != 0) {
        *client_fd  = accept(socket_client_fd, NULL, NULL);
        if (client_fd < 0) {
            no_breaking_exec_err(0);
        }else{
            readed = read(*client_fd,buf,1);
            if(readed < 0){
                no_breaking_exec_err(3);
            }else{
                if(buf[0] =='s'){
                    /*comando STORE */
                    comm_thread(&store,client_fd);
                }else if(buf[0] == 'c'){
                    /*comand CORRUPT*/
                    comm_thread(&corrupt,client_fd);
                }else if(buf[0] == 'S'){
                    /*comand SEARCH*/
                    /*comm_thread(funzione per lo storage);*/
                }else if(buf[0] == 'l'){
                    /*comand LIST*/
                    /*comm_thread(funzione per ricevere la lista);*/
                }else if(buf[0] == 'e'){
                    /*comand EXIT*/
                    /*comm_thread(chiude tutto);*/
                }
            }
        }
    }
}