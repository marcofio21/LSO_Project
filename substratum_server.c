#include "substratum_server.h"

int     dim_buf_err = 128;
char    *buf_err;

int errno;

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
        temp_conv = 0;
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
    int unchecked_val = -1;

    buf_err = malloc(dim_buf_err * sizeof(char));

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
        sprintf(buf_err,"ERR_CONF_FILE_NOT_RIGHT_FORMAT");
        write(2,buf_err,strlen(buf_err));
        free(buf_err);
        buf_err = NULL;
        free(ret);
        return(NULL);
    }

    ret = malloc(sizeof(server_addr));
    ret->addr = addr;
    ret->port = atoi(port);

    return(ret);
}


//Genera un socket in ascolto con BINDING

int             create_socket(int port, char *ip) {

    int sockfd = -1;
    int acceptfd = -1;
    int check_aton = 0;
    int check = -1;

    struct sockaddr_in server_addr ;

    buf_err = malloc(dim_buf_err * sizeof(char));

    //riempio la struttura serverAddr che serve per identificare un servizio di rete(server)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((uint16_t)port);
    check_aton = inet_aton(ip,&server_addr.sin_addr);


    //conversione dot a binary
    if (check_aton == 0) {
        sprintf(buf_err, "ERR_CONV_ADDR_ATON");
        write(2, buf_err, strlen(buf_err));
        free(buf_err);
        exit(-1);
    }

    /*server_addr.sin_addr.s_addr = htons((uint16_t )&(server_addr.sin_addr));*/

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        sprintf(buf_err, "ERR_SOCKET");
        write(2, buf_err, strlen(buf_err));
        free(buf_err);
        exit(-1);
    }

    check = bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (check < 0) {
        sprintf(buf_err, "ERR_BIND");
        write(2, buf_err, strlen(buf_err));
        free(buf_err);
        exit(-1);
    }


    check = listen(sockfd, 200);
    if (check < 0) {
        sprintf(buf_err, "ERR_LISTEN");
        write(2, buf_err, strlen(buf_err));
        free(buf_err);
        exit(-1);
    }

    return(sockfd);

}


//Controllo Connessione con altri server

void *          thread_oth_server_job(void *server_node){
    int                 connected                       = -1;
    int                 fd_socket                       = -1;

    check_servers_node    *node_to_elab                 = server_node;
    struct sockaddr_in    *test_conn_serv               = malloc(sizeof(struct sockaddr_in));

    test_conn_serv->sin_family = AF_INET;
    test_conn_serv->sin_port = htons((uint16_t)node_to_elab->server->port);
    inet_aton(node_to_elab->server->addr,&test_conn_serv->sin_addr);

    fd_socket = socket(PF_INET,SOCK_STREAM,0);

    connected = connect(fd_socket, (struct sockaddr *)test_conn_serv, sizeof(*test_conn_serv));

    if(connected == -1) {
        node_to_elab->status = 1;
    }else {
        node_to_elab->status = 0;
        close(fd_socket);
    }

    return(NULL);
}

pthread_t       thread_oth_server(check_servers_node *server_to_check){
    int check = 0;

    pthread_t tid_check_conn_thread;

    check = pthread_create(&tid_check_conn_thread,NULL, &thread_oth_server_job,server_to_check);
    if(check<0){exit(-1);}


    return (tid_check_conn_thread);
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


//Sincronizzazione con gli altri Server.

void *          commission_comm_server(void *server_addr_in){
    int         err_buf_dim     = 128;
    char        *err_buf        = malloc(err_buf_dim * sizeof(char));
    int         buf_dim         = 30;
    char        *buf            = malloc(buf_dim * sizeof(char));

    int         sockfd          = -1;
    int         entryfd         = -1;

    ssize_t     n_byte          = 0;

    server_addr  *server_addr       = server_addr_in;

    //creazione del socket
    sockfd=create_socket(server_addr->port, server_addr->addr);
    if(sockfd<0){
        sprintf(err_buf,"ERR_CREATE_SOCKET_SERVER_COMM\n");
        write(2,err_buf,strlen(err_buf));
        free(err_buf);
        exit(-1);
    }

    while(1){
        entryfd = accept(sockfd,NULL,NULL);
        if(entryfd<0){
            write(2,"\n\nerror_connect\n\n",17);
            exit(-1);
        }else{

            n_byte = read(entryfd, buf, (size_t) buf_dim);
            if (entryfd < 0) {
                sprintf(err_buf, "ERR_READ_SERVER_COMM\nERRNO : %d\n", errno);
                write(2, err_buf, strlen(err_buf));
                free(err_buf);
                exit(-1);
            }
        }
        //vanno aggiunti comandi
    }
}

int             comm_thread(server_addr *addr_server){
    int check = 0;

    pthread_t tid;
    check = pthread_create(&tid,NULL, &commission_comm_server,addr_server);

    return(check);
}
