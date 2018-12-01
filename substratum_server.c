#include "substratum_server.h"

int     dim_buf_err = 128;
char    *buf_err;

int checked_p_range_input(char *input_string, int a, int b){
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
        if((flag_err == 0) && (input > a && (b==0 || input <= b))){
            return(input);
        }else {
            return(-1);
        }
    }
}

int pow_int(int a, int exp){
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

int check_dot_addr(char *input, int length){
    int ret         = -1;
    int check       = 0;

    int dots = 0;
    int colon = 0;
    int n_of_int = 3;
    int i = 0;
    int len_check = length - 1; // decrementandolo controllo che alla fine del controllo non vi siano rimasti caratteri, altrimenti vuol dire che la stringa ha caratteri dopo la stringa corretta e quindi è sbagliata

    //valore di appoggio per il singolo carattere convertito ad int
    int unchecked_val = -1;

    buf_err = malloc(dim_buf_err * sizeof(char));

    //la stringa con l'indirizzo non può cominciare con un punto.
    if (input[0] == '.') {check = -1;}

    while(i<length && dots<3 && colon<2 && check == 0) {
        //controllo che ci siano dei numeri e non altri caratteri.
        unchecked_val = input[i] - '0';

        if (unchecked_val >= 0 && unchecked_val <= 9) {
            //trovato carattere numerico, quindi incremento per il prossimo carattere e decremento il numero di interi consentiti per quella sezione dell'indirizzo
            --n_of_int;
            if (n_of_int < 0) { check = -1; }
        }else if(unchecked_val == -2){
            //trovato '.' e quindi aumento il numero di dots incontrati e controllo se sto nelle specifiche, ovvero al più due punti e non sia seguito da un altro punto
            ++dots;
            if((input[i+1] - '0') == -2 || dots>2){check = -1;}
            n_of_int = 3;
        }else if(unchecked_val == 10){
            //trovato ':', e quindi svolgo gli stessi controlli di sopra, ma modificando il numero di caratteri numerici che devono seguirlo
            ++colon;
            if(colon > 1){check = -1;}
            n_of_int = 4;
        }else{check = -1;}
        ++i;
        --len_check;
    }

    if(check != 0 ){
        sprintf(buf_err,"ERR_CONF_FILE_NOT_RIGHT_FORMAT");
        write(2,buf_err,strlen(buf_err));
        free(buf_err);
        buf_err = NULL;
        return(-1);
    }

    return(ret);
}