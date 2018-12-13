#include "substratum_server.h"

//Lista server per la comunicazione interna.
head_list               *server_address_list    = NULL;
server_addr             *this_server_addr       = NULL;

int     *data_from_server_list = NULL;  // per la search; quando viene eseguita, gli altri server devono comunicare la loro coppia di chiave x e che valore ha associato y.

//LISTA COPPIE.
head_list *data_couples_list =  NULL;

int main(int argc, char *argv[]) {
    int i = 0;
    int j = 0;
    int offset = 0;

    int fd = -1;
    int check = 0;
    int num_byte = -1;

    int num_server = 0;

    size_t buf_dim = 350;
    int err_buf_dim = 128;
    size_t addr_lenght = 21;

    char *err_buf = malloc(err_buf_dim * sizeof(char));
    char *buf = malloc(buf_dim * sizeof(char));
    char *readed_addr = malloc(addr_lenght * sizeof(char));

    server_addr *ret_addr = NULL;

    if (argc != 3) {
        sprintf(err_buf, "Wrong number of args\n");
        write(2, err_buf, strlen(err_buf));
        exit(-1);
    }
    if ((checked_p_range_input(argv[2], 0, 9999) < 0)) {
        sprintf(err_buf, "Invalid PORT\n");
        write(2, err_buf, strlen(err_buf));
        exit(-1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        sprintf(err_buf, "ERR_OPEN_FILE\n\n");
        write(2, err_buf, strlen(err_buf));
        exit(-1);
    }

    server_address_list = create_list();

    while (read(fd, buf + i, 1) == 1) {
        if (buf[i] == '\n' || buf[i] == 0x0) {
            num_byte = i - offset;
            strncpy(readed_addr, buf + offset, (size_t) num_byte);
            ret_addr = check_dot_addr(readed_addr, num_byte);

            if (!ret_addr) { exit(-1); }
            offset = i + 1;

            if (j >= 1) {
                server_address_list = insert_node(server_address_list, ret_addr);
            } else {
                this_server_addr = ret_addr;
                ++j;
            }
        }
        ++i;
    }

    //socket in ascolto rispetto gli altri server.
    check = comm_thread(this_server_addr);
    ++num_server;
    if (check != 0) {
        sprintf(err_buf, "ERR_CREATE_THREAD\n");
        write(2, err_buf, strlen(err_buf));
        free(err_buf);
        err_buf = NULL;
        exit(-1);
    }

    server_address_list = create_main_panel(server_address_list);


    int retry_conn_count = 0;
    int conn = 1;
    while(retry_conn_count < 10 && conn == 1){
        ++retry_conn_count;
        sprintf(buf,"Tentativo %d di Connessione con gli altri server...\n",retry_conn_count);
        write(0,buf,strlen(buf));
        bzero(buf,sizeof(*buf));

        i = 0;
        do {
            /*CREAZIONE NUOVO THREAD : fa la connect verso uno dei server della lista*/
            thread_oth_server((main_panel.panel) + i);
            ++i;
        }while(i < main_panel.num_of_rows);

        sleep(2);

        conn = check_conn_to_other_server();

        if(conn == 1) {
            sleep(10);
        }
    }

    if(retry_conn_count<10) {
        sprintf(buf, "\n\nConnessione Stabilita!\n\n");
        write(0,buf,strlen(buf));
        bzero(buf,sizeof(*buf));
    }else{
        sprintf(buf, "\n\nImpossibile stabile connessione con gli altri server.\n");
        write(0,buf,strlen(buf));
        bzero(buf,sizeof(*buf));
        exit(-1);
    }
    data_couples_list = create_list();

    while(1){;}  // Da rimuovere
    /* Gestione threads per i client
     * Gestione comandi client */
}