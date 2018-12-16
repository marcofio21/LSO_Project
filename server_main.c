#include "substratum_server.h"

//Lista server per la comunicazione interna.
server_addr             *this_server_inner_addr         = NULL;
server_addr             *this_server_client_addr        = NULL;
int                     socket_client_fd                      = -1;
int                     *client_fd;

//LISTA COPPIE.
head_list *data_couples_list =  NULL;

int main(int argc, char *argv[]) {
    int i = 0;
    int j = 0;
    int offset = 0;

    int fd = -1;
    int check = 0;
    int num_byte = -1;

    size_t buf_dim = 350;
    int err_buf_dim = 128;
    size_t addr_lenght = 21;

    char *err_buf = malloc(err_buf_dim * sizeof(char));
    char *buf = malloc(buf_dim * sizeof(char));
    char *readed_addr = malloc(addr_lenght * sizeof(char));

    server_addr *ret_addr = NULL;

    if (argc != 3) {
        breaking_exec_err(0);
    }

    int t_port = (checked_p_range_input(argv[2], 0, 9999));
    if (t_port < 0) {
        breaking_exec_err(1);
    }

    //Memorizzo nella struttura preposta, l'indirizzo e la porta del server a cui deve connettersi il client
    this_server_client_addr             = malloc(sizeof(server_addr));
    this_server_client_addr->addr       = "127.0.0.1";
    this_server_client_addr->port       = t_port;

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        breaking_exec_err(2);
    }

    //Creo la lista degli altri server e del loro stato.
    servers_check_list = create_list();
    check_servers_node *temp    = NULL;

    while (read(fd, buf + i, 1) == 1) {
        if (buf[i] == '\n' || buf[i] == 0x0) {
            num_byte = i - offset;
            strncpy(readed_addr, buf + offset, (size_t) num_byte);
            ret_addr = check_dot_addr(readed_addr, num_byte);

            if (!ret_addr) { exit(-1); }

            offset = i + 1;
            if (j >= 1) {
                temp = create_new_node(ret_addr,1,j);
                servers_check_list = insert_node(servers_check_list, temp);
            } else {
                this_server_inner_addr = ret_addr;
                ++j;
            }
        }
        ++i;
    }

    //socket in ascolto rispetto gli altri server.
    check = first_conn_interface();
    if(check!=1){
        breaking_exec_err(3);
    }

    data_couples_list = create_list();

    socket_client_fd = create_socket(this_server_client_addr->port,this_server_client_addr->addr);
    if (socket_client_fd != 0) {
        breaking_exec_err(4);
    }

    client_fd = malloc(sizeof(int));
    while(check != 0) {
        *client_fd = accept(socket_client_fd, NULL, NULL);
        if (client_fd < 0) {
            no_breaking_exec_err(0);
        }else{
            check = comm_thread(client_fd);
        }
    }
}