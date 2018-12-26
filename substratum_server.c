#include "substratum_server.h"

int errno;

typedef struct comm_temp_struct temp_s;

struct comm_temp_struct{
    int             serv_fd;
    server_addr     *addr_serv;
    mem_data        *couple;
};

//Funzioni di Controllo input

int             checked_p_range_input(char *input_string, int a, int b){
    char string[64];
    bzero(string,sizeof(string));

    int input;
    int i;
    int exp;
    int temp_conv;
    int flag_err;

    while(1){
        input = 0;
        i = 0;
        exp = 0;
        flag_err = 0;

        strcpy(string,input_string);

        while(string[i]!=EOF && string[i]!='\n' && string[i]!='\0') {
            i++;
        }

        --i;

        if(i>=0) {
            while (i >= 0) {
                temp_conv = string[i] - '0';
                if(temp_conv > 9 || temp_conv < 0){
                    flag_err = 1;
                    break;
                }
                input += temp_conv * (pow_int(10, exp));
                exp++;
                i--;
            }
        }
        if((flag_err == 0) && (input > a && (input <= b))){
            return(input);
        }else {
            return(-1);
        }
    }
}

int             pow_int(int a, int exp){
    int ret = a;
    if(exp > 0){
        for(int i=2; i<=exp; i++) {
            ret = ret * a;
        }
    }else if(exp == 0){
        ret = 1;
    }
    return(ret);
}

server_addr *    check_dot_addr(char *input, int length){
    int     check           = 0;

    int     dots            = 0;
    int     colon           = 0;
    int     n_of_int        = 3;
    int     i               = 0;
    int     port_i          = 0;


    server_addr *ret         = NULL;
    char *addr              = malloc(15 * sizeof(char));
    bzero(addr,sizeof(*addr));
    char *port              = malloc(4 * sizeof(char));

    //valore di appoggio per il singolo carattere convertito ad int
    int unchecked_val;

    //la stringa con l'indirizzo non può cominciare con un punto.
    if (input[0] == '.') {check = -1;}

    while(i<length && dots<=3 && colon<=1 && check == 0) {
        //controllo che ci siano dei numeri e non altri caratteri.
        unchecked_val = input[i] - '0';

        if (unchecked_val >= 0 && unchecked_val <= 9) {
            //trovato carattere numerico, quindi incremento per il prossimo carattere e decremento il numero di interi consentiti per quella sezione dell'indirizzo
            --n_of_int;
            if(colon == 0){
                addr[i] = input[i];  //non è stato ancora trovato il doppio punto, quindi continuo a memorizzarmi la parte dell'indirizzo che è l'addr
            }else{
                port[port_i] = input[i];  //è stato trovato, quindi i seguenti 4 caratteri sono i 4 interi che compongono la porta
                ++port_i;
            }
            if (n_of_int < 0) { check = -1; }
        }else if(unchecked_val == -2){
            //trovato '.' e quindi aumento il numero di dots incontrati e controllo se sto nelle specifiche, ovvero al più due punti e non sia seguito da un altro punto
            ++dots;
            if((input[i+1] - '0') == -2|| dots>3){check = -1;}

             addr[i] = input[i];

            n_of_int = 3;
        }else if(unchecked_val == 10){
            //trovato ':', e quindi svolgo gli stessi controlli di sopra, ma modificando il numero di caratteri numerici che devono seguirlo
            ++colon;
            if(colon > 1){check = -1;}
            n_of_int = 4;
        }else{check = -1;}
        ++i;
    }

    if(check != 0){
        no_breaking_exec_err(1);
    }

    ret = malloc(sizeof(server_addr));
    ret->addr = addr;
    ret->port = atoi(port);

    return(ret);
}


//Genera un socket in ascolto con BINDING

int             create_socket(int port, char *ip) {

    int sockfd;
    int check_aton = 0;
    int check;

    struct sockaddr_in server_addr ;

    //riempio la struttura serverAddr che serve per identificare un servizio di rete(server)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((uint16_t)port);
    check_aton = inet_aton(ip,&server_addr.sin_addr);


    //conversione dot a binary
    if (check_aton == 0) {
        breaking_exec_err(5);
    }

    /*server_addr.sin_addr.s_addr = htons((uint16_t )&(server_addr.sin_addr));*/

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        breaking_exec_err(6);
    }

    check = bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (check < 0) {
        breaking_exec_err(6);
    }


    check = listen(sockfd, 200);
    if (check < 0) {
        breaking_exec_err(7);
    }

    return(sockfd);

}


//Controllo Connessione con altri server

void *          test_server_conn(check_servers_node *server_node){
    int connected;
    int fd_socket;

    struct sockaddr_in    *test_conn_serv               = malloc(sizeof(struct sockaddr_in));

    test_conn_serv->sin_family = AF_INET;
    test_conn_serv->sin_port = htons((uint16_t)server_node->server->port);
    inet_aton(server_node->server->addr,&test_conn_serv->sin_addr);

    fd_socket = socket(PF_INET,SOCK_STREAM,0);

    connected = connect(fd_socket, (struct sockaddr *)test_conn_serv, sizeof(*test_conn_serv));

    if(connected == -1) {
        server_node->status = 1;
    }else {
        server_node->status = 0;
        close(fd_socket);
    }

    return(NULL);
}

int             check_conn_to_other_server(){
    if(servers_check_list) {
        int ret = 1;
        int finded_offline_server = 0;

        int *end_list = malloc(sizeof(int));
        *end_list = 0;

        check_servers_node  *temp                = NULL;
        node_list           *reading_point       = servers_check_list->top_list;

        do {
            temp = reading_point->value;
            reading_point = reading_point->next;
            if(temp->status == 1){
                finded_offline_server = 1;
            }
        } while (reading_point && finded_offline_server == 0);

        if(finded_offline_server == 0){
            ret = 0;
        }

        return (ret);
    }else{
        return(-1);
    }
}

int             first_conn_interface(){
    int     ret                             = -1;
    int     i                               = 0;

    int     retry_conn_count                = 0;
    int     conn                            = 1;

    int     *end_list                       = malloc(sizeof(int));
            *end_list                       = 0;

    node_list               *reading_point       = NULL;

    while(retry_conn_count < 10 && conn == 1){
        ++retry_conn_count;


        retry_conn(retry_conn_count);

        i = 0;
        reading_point = servers_check_list->top_list;
        do {
            /*Si scorre l'intera lista dei server e ne controlla la disponibilità*/
            test_server_conn(reading_point->value);
            reading_point = reading_point->next;
            ++i;
        }while(reading_point);

        conn = check_conn_to_other_server();

        if(conn == 1) {
            sleep(5);
        }else if (conn == -1){
            breaking_exec_err(8);
        }
    }

    if(retry_conn_count<10) {
        ok_conn();
        ret = 1;
    }else{
        bad_conn();
        ret = -1;
    }

    return (ret);
}

server_addr *   create_list_other_server(char *conf_file_link){
    int size_buf                    = 128;
    char *buf                       = malloc(size_buf * sizeof(char));

    int i                           = 0;
    int j                           = 0;
    int fd                          = -1;

    int num_byte                    = -1;
    int offset                      = 0;

    char *readed_addr = malloc(size_buf * sizeof(char));
    server_addr *temp_addr_node     = NULL;
    server_addr *ret                = NULL;

    fd = open(conf_file_link, O_RDONLY);
    if (fd < 0) {breaking_exec_err(2);}

    //Creo la lista degli altri server e del loro stato.
    servers_check_list = create_list();
    check_servers_node *temp    = NULL;

    while (read(fd, buf + i, 1) == 1) {
        if (buf[i] == '\n' || buf[i] == 0x0) {
            num_byte = i - offset;
            strncpy(readed_addr, buf + offset, (size_t) num_byte);
            temp_addr_node = check_dot_addr(readed_addr, num_byte);

            if (!temp_addr_node) { exit(-1); }

            offset = i + 1;
            if (j >= 1) {
                temp = create_new_node(temp_addr_node,1,j);
                servers_check_list = insert_node(servers_check_list, temp);
            } else {
                ret = temp_addr_node;
                ++j;
            }
        }
        ++i;
    }

    return (ret);
}

//Interfaccia che permette di creare un thread e di assegnargli il lavoro "job" che si vuole.
pthread_t       comm_thread(FJOBTHREAD *fjob_t, void *par) {
    pthread_t tid;
    pthread_create(&tid,NULL,fjob_t,par);

    return(tid);
}

char*           receive_all(int sockfd){
    ssize_t  byte           = -1;
    int      i              = 0;

    int    dim_str          = 128;
    char   *str             = malloc(dim_str*sizeof(char));

    //il while legge finché non viene restituito un valore minore del buffer,
    //cioè quando si è letto l'ultima parte della stringa, e quindi meno di 64
    //caratteri, oppure è 0.
    //Continua, se si è letto tutti i caratteri al suo interno e potrebbero essercene altri.
    while( (byte = (read(sockfd,str + i,64))) == 64){
        i += 64;
        if(i > 128){return(NULL);}
    }
    if(byte<0){return (NULL);}

    return (str);
}


//Funzioni JOB Thread per la comunicazione DAL server VERSO gli altri server

void *          store(void *socket_p){
    int             socket_fd           = *((int *)socket_p);
    mem_data        *node_ret           = NULL;
    char            *key                = NULL;
    char            *value              = NULL;

    int             *check_end_thr      = NULL;
    int             f_err               = 0;

    node_list       *reading_point      = NULL;

    if(socket_p) {
        node_list *node_to_check = malloc(sizeof(node_list)); //Nodo d'appoggio per la ricerca

        write(socket_fd, "K", 1);
        key = receive_all(socket_fd); //leggo la chiave.
        if (!key) {
            write(socket_fd, "!sk", 3);
            return (NULL);
        }

        write(socket_fd, "K", 1);

        value = receive_all(socket_fd); //leggo la value
        if (!value) {
            write(socket_fd, "!vl", 3);
            return (NULL);
        }

        if (data_couples_list) {
            void *node      = create_new_couples_node(key,value);

            if(data_couples_list->num_node > 0) {
                //cerco nella lista locale se c'è la chiave inserita e memorizzata in input.
                node_list* temp_node = malloc(sizeof(node_list));
                temp_node->value = node;
                node_ret = search_node(data_couples_list, temp_node, &comp_couples);

                temp_node->value=NULL;
                free(temp_node);
                if(node_ret){
                    write(socket_fd,"found",5);
                    return (NULL);
                }
            }


            //controllo il numero di server
            if(servers_check_list && servers_check_list->top_list) {

                //creo array per i TID dei thread per la comunicazione con gli altri server
                pthread_t arr[servers_check_list->num_node];
                temp_s *arr_t[servers_check_list->num_node];

                int i = 0;
                int comm_other_server_socketfd = 0;
                reading_point = servers_check_list->top_list;
                temp_s *t = NULL;
                do {
                    t = malloc(sizeof(temp_s));
                    t->couple = malloc(sizeof(mem_data));

                    t->couple->key = key;
                    t->couple->value = value;
                    t->addr_serv = ((check_servers_node *) reading_point->value)->server;

                    arr[i] = comm_thread(&check_store, t);
                    arr_t[i] = t;
                    i++;

                    reading_point = reading_point->next;
                    t = NULL;

                } while (reading_point);

                for (int j = 0; j < servers_check_list->num_node; j++) {
                    pthread_join(arr[j], (void **) &check_end_thr);
                    if (check_end_thr) {
                        f_err = 1;
                        break;
                    }
                }

                //Ho trovato almeno un thread che mi ha restituito errore.
                //va introdotta la logica per dare al client l'errore giusto
                if (f_err == 1) {
                    for (int k = 0; k < servers_check_list->num_node; k++) {
                        write(arr_t[k]->serv_fd, "abort", 5);
                        close(arr_t[k]->serv_fd);
                    }
                    //il client sà che qualche thread ha qualche errore
                    char *buf = malloc(20 * sizeof(char));
                    sprintf(buf, "%d", *check_end_thr);
                    write(socket_fd, "buf", 1);
                    free(buf);
                } else {
                    for (int k = 0; k < servers_check_list->num_node; k++) {
                        write(arr_t[k]->serv_fd, "K", 1);
                        close(arr_t[k]->serv_fd);

                    }
                    data_couples_list = insert_node(data_couples_list, node);
                    //il client sà che l'inserimento è andato a buon fine
                    write(socket_fd, "K", 1);
                }

            }else { // altrimenti, non ci sono altri server e quindi non devo inoltrare nulla ed inserisco direttamente.
                data_couples_list = insert_node(data_couples_list, node);
                //il client sà che l'inserimento è andato a buon fine
                write(socket_fd, "K", 1);
            }

            close(socket_fd);
        }else{
            close(socket_fd);
            breaking_exec_err(9);
        }
    }


    return (NULL);
}

void *          check_store(void *temp_struct){
    size_t  size_buf    = 128;
    char    *buf        = malloc(size_buf * sizeof(buf));
    int     *err        = malloc(sizeof(int));

    ssize_t     byte        = 0;
    *err                    = 0;
    temp_s *input = temp_struct;

    struct sockaddr_in *addr_server = malloc(sizeof(struct sockaddr_in));

    addr_server->sin_family = AF_INET;
    addr_server->sin_port   = htons((uint16_t)input->addr_serv->port);
    inet_aton(input->addr_serv->addr,&addr_server->sin_addr);

    //creato il socket con il server dato in incarico
    input->serv_fd = socket(PF_INET,SOCK_STREAM,0);

    *err = connect(input->serv_fd,(struct sockaddr *)addr_server,sizeof(*addr_server));
    if(*err < 0){
        *err = -1;
        return(err);
    }
    byte = write(input->serv_fd,"sync_store",10);
    if(byte <= 0){
        *err = -2;
        return(err);
    }

    byte = read(input->serv_fd,buf,1);
    if(byte == 0 || (strcmp(buf,"K")) != 0){
        *err = -2;
        return(err);
    }
    bzero(buf,sizeof(*buf));


    byte = write(input->serv_fd,input->couple->key,
            strlen(input->couple->key));
    if(byte <= 0){
        *err = -3;
        return(err);
    }

    byte = read(input->serv_fd,buf,1);
    if(byte == 0 || (strcmp(buf,"K")) != 0){
        *err = -3;
        return(err);
    }
    bzero(buf,sizeof(*buf));

    byte = write(input->serv_fd,input->couple->value,
            strlen(input->couple->value));
    if(byte <= 0){
        *err = -4;
        return(err);
    }
    buf = receive_all(input->serv_fd);
    //controllo sia che sia stata ricevuta Value che la coppia non sia già presente nel server contattato
    if(!buf || (strcmp(buf,"K")) != 0){
        if( (strcmp(buf,"!value")) != 0 ) {
            *err = -4;
            return (err);
        }else{
            *err = 0;
            return (err);
        }
    }
    bzero(buf,sizeof(*buf));

    byte = write(input->serv_fd,"wait",4);
    if(byte <= 0){
        *err = -5;
        return(err);
    }

    return (NULL);
}

void *          inner_comm_check(void *sock_server) {
    if (sock_server) {

        int socket_s = *((int *) sock_server);
        char *key = NULL;
        char *value = NULL;
        char *buf = NULL;

        void *temp_node = NULL;
        node_list *check_node = NULL;
        ssize_t check = 0;

        if (data_couples_list) {

            check = write(socket_s, "K", 1);
            if (check <= 0) {
                no_breaking_exec_err(6);
                return (NULL);
            }

            key = receive_all(socket_s);
            if (!key || strlen(key) == 0 || strcmp(key, "abort") == 0) {
                write(socket_s, "!K", 2);
                return (NULL);
            }

            write(socket_s, "K", 1);

            value = receive_all(socket_s);
            if (!value || strlen(value) == 0 || strcmp(value, "abort") == 0) {
                write(socket_s, "!value", 6);
                return (NULL);
            }

            temp_node = create_new_couples_node(key, value);

            write(socket_s, "K", 1);

            buf = malloc(10 * sizeof(char));
            ssize_t byte = read(socket_s, buf, 4);
            if (byte < 0 || (strcmp(buf, "wait")) != 0) {
                write(socket_s, "err_wait", 8);
                return (NULL);
            }
            free(buf);
            buf = NULL;

            //Inizio sezione critica
            buf = receive_all(socket_s);
            if (!buf || (strcmp(buf, "abort")) == 0) {
                free(temp_node);
                //chiudo la sezione critica
                return (NULL);
            }
            if ((strcmp(buf, "K")) == 0) {
                data_couples_list = insert_node(data_couples_list, temp_node);
                //chiudo la sezione critica
            }
            free(buf);
        }
    }
    return (NULL);
}

void *          inner_comm_search(void *sock_server){

    return (NULL);
}



void *          corrupt(void *socket_p){
    int             socket_fd           = *((int *)socket_p);
    mem_data        *node_ret           = NULL;
    char            *key                = NULL;
    char            *new_value          = NULL;


    if(socket_p) {
        node_list *node_to_check = malloc(sizeof(node_list)); //Nodo d'appoggio per la ricerca

        write(socket_fd, "K", 1);
        key = receive_all(socket_fd); //leggo la chiave.
        if (!key) {
            write(socket_fd, "!sk", 3);
            return (NULL);
        }

        write(socket_fd, "K", 1);

        new_value = receive_all(socket_fd); //leggo la value
        if (!new_value) {
            write(socket_fd, "!vl", 3);
            return (NULL);
        }

        if (data_couples_list) {
            void *node = create_new_couples_node(key,NULL);

            if (data_couples_list->num_node > 0) {
                //cerco nella lista locale se c'è la chiave inserita e memorizzata in input.
                node_list *temp_node = malloc(sizeof(node_list));
                temp_node->value = node;
                node_ret = search_node(data_couples_list, temp_node, &comp_couples);

                temp_node->value = NULL;
                free(temp_node);
                if (node_ret) {
                    char *t = node_ret->value;
                    node_ret->value = new_value;
                    free(t);

                    write(socket_fd, "K",1);
                }



            }
              //copre il caso di lista vuota o coppia non trovata
              if(!node_ret) {
                  write(socket_fd,"no_found",8);

              }

            close(socket_fd);
        }
    }

    return(NULL);
}



void *          list(void *socket_p){
    ssize_t check;
    char *buf = NULL;
    int  socket;

    if(socket_p){
        socket = *((int *)socket_p);
        buf = malloc(128 * sizeof(char));

        //invio numero di nodi lista locale
        sprintf(buf,"%d",data_couples_list->num_node);
        check = write(socket,buf,strlen(buf));
        if(check<0){return(NULL);}
        bzero(buf,sizeof(*buf));

        buf = receive_all(socket);
        if(!buf || (strcmp(buf,"K"))!= 0){ return (NULL);}
        bzero(buf,sizeof(*buf));

        //se la lista non è vuota la scorro e leggo i nodi
        if(data_couples_list->num_node > 0) {
            node_list   *read_p = data_couples_list->top_list;;
            mem_data    *t      = read_p->value;

            for (int i = 0; i < data_couples_list->num_node; i++) {

                check = write(socket,t->key,strlen(t->key));//invio chiave
                if(check<0){return(NULL);}

                buf = receive_all(socket);
                if(!buf || (strcmp(buf,"K"))!= 0){ return (NULL);}
                bzero(buf,sizeof(*buf));

                check = write(socket,t->value,strlen(t->value));//invio valore
                if(check<0){return(NULL);}

                buf = receive_all(socket);
                if(!buf || (strcmp(buf,"K"))!= 0){ return (NULL);}
                bzero(buf,sizeof(*buf));

                read_p = read_p->next;
                if(read_p){
                    t      = read_p->value;
                }

            }
        }
        close(socket);
    }


    return(NULL);
}


// Funzioni JOB Thread per il decifrare i messaggi dagli altri server

void *          lister_from_other_server(void *socket) {
    char *buf = malloc(128 * sizeof(char));
    int *inn_serv_fd = NULL;

    ssize_t readed = 0;

    do {
        inn_serv_fd = malloc(sizeof(int));
        *inn_serv_fd = accept(*((int *) (socket)), NULL, NULL);
        if (*inn_serv_fd < 0) {
            no_breaking_exec_err(3);
        } else {
            readed = read(*inn_serv_fd, buf, 128);
            if (readed < 0) {
                no_breaking_exec_err(3);
            } else {
                if ((strcmp(buf, "sync_store")) == 0) {
                    comm_thread(&inner_comm_check, (void *) inn_serv_fd);
                } else if ((strcmp(buf, "sync_search")) == 0) {
                    comm_thread(&inner_comm_search, (void *) inn_serv_fd);
                }
            }
            bzero(buf, sizeof(*buf));
        }
    } while (*inn_serv_fd != -1);

    return (NULL);
}