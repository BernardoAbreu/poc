#include "util.h"


std::string compress_array(int *arr, int size){
    std::string result;
    int n, r;
    char c = 0;

    bool insert= 0;
    for(int i = 0; i < size; i++){
        n = arr[i];

        while(n){
            r = n % 10;
            n /= 10;
            if(insert){
                c |= ((char)r & 0x0F);
                result += c;
            }
            else{
                c = (char)(r << 4);
            }
            insert = !insert;
        }

        if(insert){
            c |= (0xA & 0x0F);
            result += c;
        }
        else{
            c  = 0xA << 4;
        }
        insert = !insert;

    }
    if(insert){
        c |= (0xB & 0x0F);
        result += c;
    }

    return result;
}

std::vector<int> decompress_string(std::string &s){
    std::vector<int> result;
    int n = 0;
    int n_size = 1;
    char c[2];
    for(unsigned int i = 0; i < s.size(); i++){
        char co = s[i];
        c[0] = 0xF & (co >> 4);
        c[1] = (co & 0xF);

        for(int j = 0; j < 2; j++){
            if(c[j] == 0xA){
                result.push_back(n);
                n = 0;
                n_size = 1;
            }
            else if(c[j] != 0xB){
                n += (((int)c[j]) * n_size);
                n_size *= 10;
            }
        }
    }

    return result;
}
