#include "substratum_server.h"

int errno;

typedef struct comm_temp_struct temp_s;

struct comm_temp_struct {
    int serv_fd;
    server_addr *addr_serv;
    mem_data *couple;
};

//Funzioni di Controllo input

int checked_p_range_input(char *input_string, int a, int b) {
    char string[64];
    bzero(string, strlen(string));

    int input;
    int i;
    int exp;
    int temp_conv;
    int flag_err;

    while (1) {
        input = 0;
        i = 0;
        exp = 0;
        flag_err = 0;

        strcpy(string, input_string);

        while (string[i] != EOF && string[i] != '\n' && string[i] != '\0') {
            i++;
        }

        --i;

        if (i >= 0) {
            while (i >= 0) {
                temp_conv = string[i] - '0';
                if (temp_conv > 9 || temp_conv < 0) {
                    flag_err = 1;
                    break;
                }
                input += temp_conv * (pow_int(10, exp));
                exp++;
                i--;
            }
        }
        if ((flag_err == 0) && (input > a && (input <= b))) {
            return (input);
        } else {
            return (-1);
        }
    }
}

int pow_int(int a, int exp) {
    int ret = a;
    if (exp > 0) {
        for (int i = 2; i <= exp; i++) {
            ret = ret * a;
        }
    } else if (exp == 0) {
        ret = 1;
    }
    return (ret);
}

server_addr *check_dot_addr(char *input, int length) {
    int check = 0;

    int dots = 0;
    int colon = 0;
    int n_of_int = 3;
    int i = 0;
    int port_i = 0;


    server_addr *ret = NULL;
    char *addr = malloc(15 * sizeof(char));
    bzero(addr, strlen(addr));
    char *port = malloc(4 * sizeof(char));

    //valore di appoggio per il singolo carattere convertito ad int
    int unchecked_val;

    //la stringa con l'indirizzo non può cominciare con un punto.
    if (input[0] == '.') { check = -1; }

    while (i < length && dots <= 3 && colon <= 1 && check == 0) {
        //controllo che ci siano dei numeri e non altri caratteri.
        unchecked_val = input[i] - '0';

        if (unchecked_val >= 0 && unchecked_val <= 9) {
            //trovato carattere numerico, quindi incremento per il prossimo carattere e decremento il numero di interi consentiti per quella sezione dell'indirizzo
            --n_of_int;
            if (colon == 0) {
                addr[i] = input[i];  //non è stato ancora trovato il doppio punto, quindi continuo a memorizzarmi la parte dell'indirizzo che è l'addr
            } else {
                port[port_i] = input[i];  //è stato trovato, quindi i seguenti 4 caratteri sono i 4 interi che compongono la porta
                ++port_i;
            }
            if (n_of_int < 0) { check = -1; }
        } else if (unchecked_val == -2) {
            //trovato '.' e quindi aumento il numero di dots incontrati e controllo se sto nelle specifiche, ovvero al più due punti e non sia seguito da un altro punto
            ++dots;
            if ((input[i + 1] - '0') == -2 || dots > 3) { check = -1; }

            addr[i] = input[i];

            n_of_int = 3;
        } else if (unchecked_val == 10) {
            //trovato ':', e quindi svolgo gli stessi controlli di sopra, ma modificando il numero di caratteri numerici che devono seguirlo
            ++colon;
            if (colon > 1) { check = -1; }
            n_of_int = 4;
        } else { check = -1; }
        ++i;
    }

    if (check != 0) {
        no_breaking_exec_err(1);
    }

    ret = malloc(sizeof(server_addr));
    ret->addr = addr;
    ret->port = atoi(port);

    return (ret);
}


//Genera un socket in ascolto con BINDING

int create_socket(int port, char *ip) {

    int sockfd;
    int check_aton = 0;
    int check;

    struct sockaddr_in server_addr;

    //riempio la struttura serverAddr che serve per identificare un servizio di rete(server)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((uint16_t) port);
    check_aton = inet_aton(ip, &server_addr.sin_addr);


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

    return (sockfd);

}


//Controllo Connessione con altri server

void *test_server_conn(check_servers_node *server_node) {
    int connected;
    int fd_socket;

    struct sockaddr_in *test_conn_serv = malloc(sizeof(struct sockaddr_in));

    test_conn_serv->sin_family = AF_INET;
    test_conn_serv->sin_port = htons((uint16_t) server_node->server->port);
    inet_aton(server_node->server->addr, &test_conn_serv->sin_addr);

    fd_socket = socket(PF_INET, SOCK_STREAM, 0);

    connected = connect(fd_socket, (struct sockaddr *) test_conn_serv, sizeof(*test_conn_serv));

    if (connected == -1) {
        server_node->status = 1;
    } else {
        server_node->status = 0;
        close(fd_socket);
    }

    return (NULL);
}

int check_conn_to_other_server() {
    if (servers_check_list) {
        int ret = 1;
        int finded_offline_server = 0;

        int *end_list = malloc(sizeof(int));
        *end_list = 0;

        check_servers_node *temp = NULL;
        node_list *reading_point = servers_check_list->top_list;

        do {
            temp = reading_point->value;
            reading_point = reading_point->next;
            if (temp->status == 1) {
                finded_offline_server = 1;
            }
        } while (reading_point && finded_offline_server == 0);

        if (finded_offline_server == 0) {
            ret = 0;
        }

        return (ret);
    } else {
        return (-1);
    }
}

int first_conn_interface() {
    int ret = -1;
    int i = 0;

    int retry_conn_count = 0;
    int conn = 1;

    int *end_list = malloc(sizeof(int));
    *end_list = 0;

    node_list *reading_point = NULL;

    while (retry_conn_count < 10 && conn == 1) {
        ++retry_conn_count;


        retry_conn(retry_conn_count);

        i = 0;
        reading_point = servers_check_list->top_list;
        do {
            /*Si scorre l'intera lista dei server e ne controlla la disponibilità*/
            test_server_conn(reading_point->value);
            reading_point = reading_point->next;
            ++i;
        } while (reading_point);

        conn = check_conn_to_other_server();

        if (conn == 1) {
            sleep(10);
        } else if (conn == -1) {
            breaking_exec_err(8);
        }
    }

    if (retry_conn_count < 10) {
        ok_conn();
        ret = 1;
    } else {
        bad_conn();
        ret = -1;
    }

    return (ret);
}

server_addr *create_list_other_server(char *conf_file_link) {
    int size_buf = 128;
    char *buf = malloc(size_buf * sizeof(char));

    int i = 0;
    int j = 0;
    int fd = -1;

    int num_byte = -1;
    int offset = 0;

    server_addr *temp_addr_node = NULL;
    server_addr *ret = NULL;

    fd = open(conf_file_link, O_RDONLY);
    if (fd < 0) { breaking_exec_err(2); }

    //Creo la lista degli altri server e del loro stato.
    servers_check_list = create_list();
    check_servers_node *temp = NULL;

    while (read(fd, buf + i, 1) == 1) {
        if (buf[i] == '\n' || buf[i] == '\0') {

            buf[i] = '\0';
            temp_addr_node = check_dot_addr(buf,(int)(strlen(buf)));

            if (!temp_addr_node) { exit(-1); }

            if (j >= 1) {
                temp = create_new_node(temp_addr_node, 1, j);
                servers_check_list = insert_node(servers_check_list, temp);
            } else {
                ret = temp_addr_node;
                ++j;
            }

            bzero(buf,strlen(buf));
            i = 0;

        }else {
            ++i;
        }
    }

    return (ret);
}

//Interfaccia che permette di creare un thread e di assegnargli il lavoro "job" che si vuole.
pthread_t comm_thread(FJOBTHREAD *fjob_t, void *par) {
    int check=-1;
    pthread_t tid;
    check=pthread_create(&tid, NULL, fjob_t, par);
    if(check!=0){
        breaking_exec_err(10);
    }
    return (tid);
}

char *receive_all(int sockfd, char *buf) {
    ssize_t byte = -1;
    int i = 0;

    while ((byte = (read(sockfd, buf + i, 64))) == 64) {
        i += 64;
        if (i > 128) { return (NULL); }
    }
    if (byte < 0) {
        return (NULL);
    }else{
        buf[byte] = '\0';
    }

    return (buf);
}


//Funzioni JOB Thread per la comunicazione DAL server VERSO gli altri server

void *store(void *socket_p) {
    int socket_fd = *((int *) socket_p);
    node_list *node_ret = NULL;

    char *key = NULL;
    char *value = NULL;

    int *check_end_thr = NULL;
    int f_err = 0;

    node_list *reading_point = NULL;

    if (socket_p) {
        key = malloc(128 * sizeof(char));
        bzero(key,strlen(key));
        value = malloc(128 * sizeof(char));
        bzero(value,strlen(value));


        write(socket_fd, "K", 1);
        key = receive_all(socket_fd, key); //leggo la chiave.
        if (!key) {
            write(socket_fd, "!sk", 3);
            return (NULL);
        }

        write(socket_fd, "K", 1);

        value = receive_all(socket_fd, value); //leggo la value
        if (!value) {
            write(socket_fd, "!vl", 3);
            return (NULL);
        }

        if (data_couples_list) {
            void *node = create_new_couples_node(key, value);

            //MUTEX LOCKED
            pthread_mutex_lock(data_couples_list->mutex);

            if (data_couples_list->num_node > 0) {

                //cerco nella lista locale se c'è la chiave inserita e memorizzata in input.
                node_list *temp_node = malloc(sizeof(node_list));
                temp_node->value = node;

                node_ret = search_node(data_couples_list, temp_node, &comp_couples);

                temp_node->value = NULL;
                free(temp_node);
                if (node_ret) {
                    write(socket_fd, "found", 5);

                    //MUTEX UNLOCKED
                    pthread_mutex_unlock(data_couples_list->mutex);

                    return (NULL);
                }
            }

            //MUTEX UNLOCKED
            pthread_mutex_unlock(data_couples_list->mutex);

            //controllo il numero di server
            if (servers_check_list && servers_check_list->top_list) {

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

                /*for (int j = 0; j < servers_check_list->num_node; j++)*/
                int j = 0;
                int tot_n = 0;
                while(!check_end_thr){
                    pthread_join(arr[j], (void **) &check_end_thr);
                    if (!check_end_thr && (check_end_thr && *check_end_thr != -7)) {
                        f_err = 1;
                        break;
                    }else if(*check_end_thr == -7){
                        ++tot_n;
                        if(tot_n == servers_check_list->num_node){
                            break;
                        }
                    }
                    check_end_thr = NULL;
                    j++;
                }

                //Ho trovato almeno un thread che mi ha restituito errore.
                if (f_err == 1) {
                    for (int k = 0; k < servers_check_list->num_node; k++) {
                        write(arr_t[k]->serv_fd, "abort", 5);
                        close(arr_t[k]->serv_fd);
                    }
                    //il client sà che qualche thread ha qualche errore
                    char *buf = malloc(20 * sizeof(char));
                    sprintf(buf, "%d", *check_end_thr);
                    write(socket_fd, buf, strlen(buf));
                    free(buf);

                    close(socket_fd);
                    return(NULL);

                } else {

                    //Tutti i threads, sono in attesa e quindi hanno prenotato con successo la lista.
                    //Eseguo una nuova ricerca per controllare che nessuno abbia inserito tale chiave, nel frattempo dei miei controlli.
                    //Questo implica che fosse iniziata prima della store in corso.

                    //MUTEX LOCKED
                    pthread_mutex_lock(data_couples_list->mutex);

                    node_list *temp_node = malloc(sizeof(node_list));
                    temp_node->value = node;

                    node_ret = search_node(data_couples_list, temp_node, &comp_couples);

                    temp_node->value = NULL;
                    free(temp_node);
                    if (node_ret) {
                        write(socket_fd, "found", 5);

                        //la key che si voleva inserire era stata già stata inserita da un altro client, prima di questo, ma non ancora ultimata.
                        for (int k = 0; k < servers_check_list->num_node; k++) {
                            write(arr_t[k]->serv_fd, "fail", 4);
                            close(arr_t[k]->serv_fd);
                        }

                        //MUTEX UNLOCKED
                        pthread_mutex_unlock(data_couples_list->mutex);

                        return (NULL);
                    }else {

                        for (int k = 0; k < servers_check_list->num_node; k++) {
                            write(arr_t[k]->serv_fd, "K", 1);
                            close(arr_t[k]->serv_fd);
                        }

                        data_couples_list = insert_node(data_couples_list, node);

                        //MUTEX UNLOCKED
                        pthread_mutex_unlock(data_couples_list->mutex);


                        //il client sà che l'inserimento è andato a buon fine
                        write(socket_fd, "K", 1);
                    }
                }

            } else { // altrimenti, non ci sono altri server e quindi non devo inoltrare nulla ed inserisco direttamente.

                //MUTEX LOCKED
                pthread_mutex_lock(data_couples_list->mutex);

                data_couples_list = insert_node(data_couples_list, node);

                //MUTEX UNLOCKED
                pthread_mutex_unlock(data_couples_list->mutex);

                //il client sà che l'inserimento è andato a buon fine
                write(socket_fd, "K", 1);

            }

            close(socket_fd);
        } else {
            close(socket_fd);
            breaking_exec_err(9);
        }
    }


    return (NULL);
}
void *check_store(void *temp_struct) {
    size_t size_buf = 128;
    char *buf = malloc(size_buf * sizeof(buf));
    int *err = malloc(sizeof(int));

    ssize_t byte = 0;
    *err = 0;
    temp_s *input = temp_struct;

    struct sockaddr_in *addr_server = malloc(sizeof(struct sockaddr_in));

    addr_server->sin_family = AF_INET;
    addr_server->sin_port = htons((uint16_t) input->addr_serv->port);
    inet_aton(input->addr_serv->addr, &addr_server->sin_addr);

    //creato il socket con il server dato in incarico
    input->serv_fd = socket(PF_INET, SOCK_STREAM, 0);

    *err = connect(input->serv_fd, (struct sockaddr *) addr_server, sizeof(*addr_server));
    if (*err < 0) {
        *err = -1;
        return (err);
    }
    byte = write(input->serv_fd, "sync_store", 10);
    if (byte <= 0) {
        *err = -2;
        return (err);
    }

    byte = read(input->serv_fd, buf, 1);
    if (byte == 0 || (strcmp(buf, "K")) != 0) {
        *err = -2;
        return (err);
    }
    bzero(buf, strlen(buf));


    byte = write(input->serv_fd, input->couple->key,
                 strlen(input->couple->key));
    if (byte <= 0) {
        *err = -3;
        return (err);
    }

    byte = read(input->serv_fd, buf, 1);
    if (byte == 0 || (strcmp(buf, "K")) != 0) {
        *err = -3;
        return (err);
    }
    bzero(buf, strlen(buf));

    byte = write(input->serv_fd, input->couple->value,
                 strlen(input->couple->value));
    if (byte <= 0) {
        *err = -4;
        return (err);
    }
    buf = receive_all(input->serv_fd, buf);
    //controllo sia che sia stata ricevuta Value che la coppia non sia già presente nel server contattato
    if (!buf || (strncmp(buf, "K",1)) != 0) {
        if (buf && (strcmp(buf, "!value")) == 0) {
            *err = -4;
            return (err);
        } else {
            *err = 0;
            return (err);
        }
    }
    bzero(buf, strlen(buf));

    *err = -7;
    return (err);
}
void *inner_comm_check_store(void *sock_server) {
    if (sock_server) {

        int socket_s = *((int *) sock_server);
        char *key = malloc(128 * sizeof(char));
        memset(key,'\0',sizeof(*key));
        char *value = malloc(128 * sizeof(char));
        memset(value,'\0',sizeof(*value));

        char *buf = malloc(128 * sizeof(char));

        void *temp_node = NULL;
        ssize_t check = 0;

        if (data_couples_list) {


            check = write(socket_s, "K", 1);
            if (check <= 0) {
                no_breaking_exec_err(6);
                return (NULL);
            }

            key = receive_all(socket_s, key);
            if (!key || strlen(key) == 0 || strcmp(key, "fail") == 0) {
                write(socket_s, "!K", 2);
                return (NULL);
            }

            check = write(socket_s, "K", 1);
            if (check <= 0) {
                no_breaking_exec_err(6);
                return (NULL);
            }

            value = receive_all(socket_s, value);
            if (!value || strlen(value) == 0 || strcmp(value, "fail") == 0) {
                write(socket_s, "!value", 6);
                return (NULL);
            }

            temp_node = create_new_couples_node(key, value);

            check = write(socket_s, "K", 1);
            if (check <= 0) {
                no_breaking_exec_err(6);
                return (NULL);
            }

            buf = receive_all(socket_s, buf);
            if ((strncmp(buf, "K",1)) == 0) {
                //MUTEX LOCKED
                pthread_mutex_lock(data_couples_list->mutex);
                data_couples_list = insert_node(data_couples_list, temp_node);


            } else {
                free(temp_node);
                return (NULL);
            }
            //MUTEX UNLOCKED
            pthread_mutex_unlock(data_couples_list->mutex);
            free(buf);
        }
    }
    return (NULL);
}


void *corrupt(void *socket_p) {
    int socket_fd = *((int *) socket_p);
    mem_data *node_ret = NULL;
    char *key = malloc(128 * sizeof(char));
    char *new_value = malloc(128 * sizeof(char));


    if (socket_p) {

        write(socket_fd, "K", 1);
        key = receive_all(socket_fd, key); //leggo la chiave.
        if (!key) {
            write(socket_fd, "!sk", 3);
            return (NULL);
        }

        write(socket_fd, "K", 1);

        new_value = receive_all(socket_fd, new_value); //leggo la value
        if (!new_value) {
            write(socket_fd, "!vl", 3);
            return (NULL);
        }

        if (data_couples_list) {

            //MUTEX LOCKED
            pthread_mutex_lock(data_couples_list->mutex);

            node_list *node_to_check = malloc(sizeof(node_list)); //Nodo d'appoggio per la ricerca
            node_to_check->value = malloc(sizeof(mem_data));
            ((mem_data *)node_to_check->value)->key = key;


            node_ret = search_node(data_couples_list,node_to_check,&comp_couples);


            if (!node_ret) {
                write(socket_fd,"no_found", 8);
            }else{
                ((mem_data *)(((node_list *)node_ret)->value))->value = new_value;
                write(socket_fd,"K",1);
            }

            //MUTEX UNLOCKED
            pthread_mutex_unlock(data_couples_list->mutex);

        }else {
                write(socket_fd, "no_found",8);
        }
        close(socket_fd);
    }

    return (NULL);
}


void *search(void *socket_p){
    ssize_t check;

    node_list *node_ret = NULL;
    mem_data  *couple_ret = NULL;

    char *key = NULL;
    char *buf = NULL;

    mem_data *check_end_thr = NULL;


    node_list *reading_point = NULL;

    //la funzione è la medesima sia tra server che tra server-client
    if (socket_p) {

        buf = malloc(128 * sizeof(char));
        key = malloc(128 * sizeof(char));

        int socket_fd = *((int *) socket_p);

        struct sockaddr_in  client;
        socklen_t           size_sockclient = sizeof(client);

        check = getsockname(socket_fd,(struct sockaddr *)&client,&size_sockclient);
        if(check<0){return(NULL);}

        int port_entry = ntohs(client.sin_port);

        if(port_entry == client_port) {
            //Il client ha contattato il server, richiedendo la SEARCH.
            check = write(socket_fd, "K", 1);
            if(check<0){return(NULL);}

            key = receive_all(socket_fd, key); //leggo la chiave.
            if (!key) {
                write(socket_fd, "!sk", 3);
                return (NULL);
            }

            check = write(socket_fd, "K", 1);
            if(check<0){return(NULL);}


            if (data_couples_list) {
                void *node = create_new_couples_node(key, NULL);

                //MUTEX LOCKED
                pthread_mutex_lock(data_couples_list->mutex);

                if (data_couples_list->num_node > 0) {
                    //cerco nella lista locale se c'è la chiave in input.
                    node_list *temp_node = malloc(sizeof(node_list));
                    temp_node->value = node;


                    node_ret = search_node(data_couples_list, temp_node, &comp_couples);
                    if(node_ret){
                        couple_ret = (mem_data *)(node_ret->value);
                    }else {
                        //caso chiave non esiste
                        write(socket_fd, "no_found", 8);
                        temp_node->value = NULL;
                        free(temp_node);

                        //MUTEX UNLOCKED
                        pthread_mutex_unlock(data_couples_list->mutex);

                        return (NULL);
                    }

                    temp_node->value = NULL;
                    free(temp_node);

                }else{
                    //caso lista vuota
                    write(socket_fd, "no_found",8);

                    //MUTEX UNLOCKED
                    pthread_mutex_unlock(data_couples_list->mutex);

                    return(NULL);
                }

                //MUTEX UNLOCKED
                pthread_mutex_unlock(data_couples_list->mutex);

                //controllo il numero di server
                if (servers_check_list && servers_check_list->top_list) {

                    //creo array per i TID dei thread per la comunicazione con gli altri server
                    pthread_t arr[servers_check_list->num_node];

                    int i = 0;
                    reading_point = servers_check_list->top_list;
                    temp_s *t = NULL;
                    do {
                        t = malloc(sizeof(temp_s));
                        t->couple = malloc(sizeof(mem_data));

                        t->couple->key = key;
                        t->addr_serv = ((check_servers_node *) reading_point->value)->server;

                        arr[i] = comm_thread(&inner_comm_search, t);
                        i++;

                        reading_point = reading_point->next;
                        t = NULL;

                    } while (reading_point);

                    mem_data *swap_p;
                    for (int j = 0; j < servers_check_list->num_node; j++) {
                        pthread_join(arr[j], (void **) &check_end_thr);
                        swap_p = check_end_thr;
                        if (!check_end_thr ||
                            (strcmp(couple_ret->key, swap_p->key) == 0 && strcmp(couple_ret->value, swap_p->value) != 0)) {

                            write(socket_fd, "ledge_corrupt", 13);
                            close(socket_fd);

                            //dealloco tutto

                            return (NULL);
                        }
                    }

                }
                //Scrivo il valore y al client.
                check = write(socket_fd, ((mem_data *)(node_ret->value))->value, strlen(((mem_data *)(node_ret->value))->value));

                if (check < 0) {
                    //dealloco tutto
                    return (NULL);
                }
            }else{
                //dealloco tutto
                return (NULL);
            }
        }else if(port_entry == inner_port){
            //comunicazione tra server

            check = write(socket_fd, "K", 1);
            if(check<0){return(NULL);}

            key = receive_all(socket_fd, key); //leggo la chiave.
            if (!key) {
                write(socket_fd, "!sk", 3);
                return (NULL);
            }

            check = write(socket_fd, "K", 1);
            if(check<0){return(NULL);}

            if(data_couples_list){

                node_list   *temp_node = malloc(sizeof(node_list));
                mem_data    *data_to_s = malloc(sizeof(mem_data));

                data_to_s->key = key;

                temp_node->value = data_to_s;


                node_list *ret_node = search_node(data_couples_list,temp_node,&comp_couples);

                if(!ret_node){
                    write(socket_fd,"!K",2);
                    close(socket_fd);


                    return(NULL); //lista inconsistente
                }else{
                    check = write(socket_fd,((mem_data *)(ret_node->value))->value,strlen(((mem_data *)(ret_node->value))->value));
                    if(check < 0) {
                        write(socket_fd, "!K", 2);
                        close(socket_fd);

                        //MUTEX UNLOCKED
                        pthread_mutex_unlock(data_couples_list->mutex);

                        return (NULL);
                    }

                    buf = receive_all(socket_fd, buf);
                    if(!buf || strcmp(buf,"K")!= 0){
                        write(socket_fd,"!K",2);
                    }
                    bzero(buf,strlen(buf));
                }
            }else{write(socket_fd,"!K",2); close(socket_fd);}
        }
        close(socket_fd);
    }
    return(NULL);
}
void *inner_comm_search(void *sock_server) {
    mem_data *ret = NULL;

    if(sock_server) {
        char *buf = malloc(128 * sizeof(char));
        char *value = malloc(128 * sizeof(char));

        ret = malloc(sizeof(mem_data));

        ssize_t byte = 0;
        temp_s *input = sock_server;

        struct sockaddr_in *addr_server = malloc(sizeof(struct sockaddr_in));

        addr_server->sin_family = AF_INET;
        addr_server->sin_port = htons((uint16_t) input->addr_serv->port);
        inet_aton(input->addr_serv->addr, &addr_server->sin_addr);

        //creato il socket con il server dato in incarico
        input->serv_fd = socket(PF_INET, SOCK_STREAM, 0);

        byte = connect(input->serv_fd, (struct sockaddr *) addr_server, sizeof(*addr_server));
        if (byte < 0) {close(input->serv_fd); return (NULL);}
        byte = write(input->serv_fd, "search", 6);
        if (byte <= 0) {close(input->serv_fd); return(NULL);}

        buf = receive_all(input->serv_fd, buf);
        if(!buf || strcmp(buf,"K") != 0){close(input->serv_fd); return(NULL);}
        bzero(buf,strlen(buf));

        //scrivo valore : x

        byte = write(input->serv_fd,input->couple->key,strlen(input->couple->key));
        if (byte <= 0) {close(input->serv_fd); return(NULL);}

        buf = receive_all(input->serv_fd, buf);
        if(!buf || strncmp(buf,"K",1) != 0){close(input->serv_fd); return(NULL);}
        bzero(buf,strlen(buf));

        //Ricezione valore : y

        value = receive_all(input->serv_fd, value);
        if(!value && strcmp(value,"") == 0){
            write(input->serv_fd,"!K",2);
            close(input->serv_fd);
            return(NULL);
        }

        byte = write(input->serv_fd,"K",1);
        if (byte <= 0) {close(input->serv_fd);}

        //Fine Ricezione

        ret->key = input->couple->key;
        ret->value = value;
    }

    return (ret);
}


void *list(void *socket_p) {
    ssize_t check;
    char *buf;
    int socket;

    if (socket_p) {
        socket = *((int *) socket_p);
        buf = malloc(128 * sizeof(char));

        //MUTEX LOCKED
        pthread_mutex_lock(data_couples_list->mutex);

        //invio numero di nodi lista locale
        sprintf(buf, "%d", data_couples_list->num_node);
        check = write(socket, buf, strlen(buf));
        if (check < 0) {

            //MUTEX UNLOCKED
            pthread_mutex_unlock(data_couples_list->mutex);

            return (NULL);
        }
        bzero(buf, strlen(buf));

        buf = receive_all(socket, buf);
        if (!buf || (strcmp(buf, "K")) != 0) {

            //MUTEX UNLOCKED
            pthread_mutex_unlock(data_couples_list->mutex);
            return (NULL);
        }
        bzero(buf, strlen(buf));

        //se la lista non è vuota la scorro e leggo i nodi
        if (data_couples_list->num_node > 0) {
            node_list *read_p = data_couples_list->top_list;;
            mem_data *t = read_p->value;

            for (int i = 0; i < data_couples_list->num_node; i++) {

                check = write(socket, t->key, strlen(t->key));//invio chiave
                if (check < 0) {

                    //MUTEX UNLOCKED
                    pthread_mutex_unlock(data_couples_list->mutex);

                    return (NULL);
                }

                buf = receive_all(socket, buf);
                if (!buf || (strcmp(buf, "K")) != 0) {

                    //MUTEX UNLOCKED
                    pthread_mutex_unlock(data_couples_list->mutex);

                    return (NULL);
                }
                bzero(buf, strlen(buf));

                check = write(socket, t->value, strlen(t->value));//invio valore
                if (check < 0) {

                    //MUTEX UNLOCKED
                    pthread_mutex_unlock(data_couples_list->mutex);

                    return (NULL);
                }

                buf = receive_all(socket, buf);
                if (!buf || (strcmp(buf, "K")) != 0) {

                    //MUTEX UNLOCKED
                    pthread_mutex_unlock(data_couples_list->mutex);

                    return (NULL);
                }
                bzero(buf,strlen(buf));

                read_p = read_p->next;
                if (read_p) {
                    t = read_p->value;
                }

            }
        }

        //MUTEX UNLOCKED
        pthread_mutex_unlock(data_couples_list->mutex);

        close(socket);
    }


    return (NULL);
}


// Funzioni JOB Thread per il decifrare i messaggi dagli altri server

void *lister_from_other_server(void *socket) {
    char *buf = malloc(128 * sizeof(char));
    int *inn_serv_fd = NULL;

    ssize_t readed = 0;

    struct sockaddr_in  client;
    socklen_t           size_sockclient = sizeof(client);

    do {
        inn_serv_fd = malloc(sizeof(int));
        *inn_serv_fd = accept(*((int *) (socket)), NULL, NULL);

        getsockname(*((int *) (socket)),(struct sockaddr *)&client,&size_sockclient);

        if (*inn_serv_fd < 0) {
            no_breaking_exec_err(3);
        } else {
            readed = read(*inn_serv_fd, buf, 128);
            /*write(1,"lettura per syncStore: ",strlen("lettura per syncStore: "));
            write(1,buf,strlen(buf));
            write(1,"\n",strlen("\n"));*/

            if (readed < 0) {
                no_breaking_exec_err(3);
            } else {
                if ((strncmp(buf, "sync_store",10)) == 0) {
                    comm_thread(&inner_comm_check_store, (void *) inn_serv_fd);
                } else if ((strncmp(buf, "search",6)) == 0) {
                    comm_thread(&search, (void *) inn_serv_fd);
                }
            }
            bzero(buf,strlen(buf));
        }

        inn_serv_fd = NULL;
    } while (!inn_serv_fd);

    return (NULL);
}