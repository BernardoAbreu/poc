#include "util.h"
#include <iostream>



std::string compress_array(int *arr, int size){
    std::string result;
    int n, r;
    char c = 0;
    // std::cout << sizeof(c) << std::endl;
    bool insert= 0;
    for(int i = 0; i < size; i++){
        n = arr[i];

        while(n){
            r = n % 10;
            n /= 10;
            if(insert){
                c |= ((char)r & 0x0F);
                // std::cout << std::hex << (short)c << ' ';
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
            // std::cout << std::hex << (int)c << ' ';
        }
        else{
            c  = 0xA << 4;
        }
        insert = !insert;
        // c  = n1 << 4 | (n2 & 0x0F);
    }
    if(insert){
        c |= (0xB & 0x0F);
        // std::cout << std::hex << (int)c << ' ';
        result += c;
    }
    // std::cout << std::endl;
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
        // std::hex << co << ' '
        // std::cout << std::dec << (int)c[0] << ' ' << (int)c[1] << std::endl;
        for(int j = 0; j < 2; j++){
            if(c[j] == 0xA){
                // std::cout << "->" << n << std::endl;
                result.push_back(n);
                n = 0;
                n_size = 1;
            }
            else if(c[j] != 0xB){
                // std::cout << (int)c[j] << ' ' << ((int)c[j]) * n_size << std::endl;
                n += (((int)c[j]) * n_size);
                // std::cout << n << std::endl;
                n_size *= 10;
            }
        }
    }

    return result;
}