#ifndef UTIL_H
#define UTIL_H


#include <sstream>
#include <iterator>
#include <string>
#include <algorithm>
// #include "util.h"

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}


template< typename T >
typename std::vector<T>::iterator insert_sorted( std::vector<T> & vec, T const& item ){
    return vec.insert(std::upper_bound( vec.begin(), vec.end(), item ), item);
}


template< typename T >
void insert_sorted( std::vector<T> & vec, T const& item, int const size){
    T item_replace = item;
    for(int i = 0; i < size-1; i++){
        if(vec[i] > item_replace){
            std::swap(vec[i], item_replace);
        }
    }
    vec[size-1] = item_replace;
}


template<typename T>
std::string join(const T &v, char separator, int max_size){
    typename T::const_iterator it;

    if(max_size == 0 || v.size() == 0) return "";

    it = v.cbegin();
    std::string key = patch::to_string(*(it++));

    for(int i = 1; it != v.cend() && i < max_size; i++, it++){
        key += (separator + patch::to_string(*it));
    }

    return key;
}

template<typename T>
std::string join(const T &v, char separator){
    typename T::const_iterator it;

    if(v.size() == 0) return "";

    it = v.cbegin();
    std::string key = patch::to_string(*(it++));

    for(; it != v.cend(); it++){
        key += (separator + patch::to_string(*it));
    }

    return key;
}


template<typename T>
std::string join(const T &v, char separator, int max_size, int remove_pos){
    typename T::const_iterator it = v.cbegin();
    std::string key = "";
    int k;
    if(v.size() > 1 && max_size){
        if(remove_pos == 0){
            key = patch::to_string(*(++it));
            k = 2;
        }
        else{
            key = patch::to_string(*(it));
            k = 1;
        }
        it++;
    }

    for(; it != v.cend() && k < (max_size + 1); k++, it++){
        if(k != remove_pos){
            key += (separator + patch::to_string(*it));
        }
    }
    return key;
}


/*
    Obtained from https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
    Author: Evan Teran
 */
template<typename T>
void split(const std::string &s, char delim, T result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;

    typename T::container_type::value_type item_type;
    while (std::getline(ss, item, delim)) {
        std::istringstream(item) >> item_type;
        *(result++) = item_type;
    }
}

template<typename T>
T split(const std::string &s, char delim) {
    T elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

#endif
