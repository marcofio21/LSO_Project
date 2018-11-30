#include "substratum_server.h"

int checked_p_range_input(int a, int b){
    char string[64];
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
        fgets(string, 64, stdin);

        while(string[i]!=EOF && string[i]!='\n') {
            i++;
        }

        i--;

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
        if((flag_err == 0) && (input >= a && (b==0 || input <= b))){
            puts("");
            return(input);
        }else {
            puts("\n\nValore non corretto. Reinserire.\n\n");
        }
    }
}

int pow_int(int a, int exp){
    int ret = a;
    if(exp > 0){
        for(int i=2; i<=exp; i++) {
            ret *= ret;
        }
    }else if(exp == 0){
        ret = 1;
    }
    return(ret);
}