#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>


std::string compress_array(int *arr, int size);

std::vector<int> decompress_string(std::string &s);


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
typename std::vector<T>::iterator insert_sorted(std::vector<T> &vec, const T &item){
    return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}


template< typename T >
void insert_sorted(std::vector<T> &vec, const T &item, int const size){
    typename std::vector<T>::iterator upper = std::upper_bound(vec.begin(), vec.begin() + size, item);
    int pos = upper - vec.begin();
    std::move_backward(upper, upper+(size-pos), upper+(size-pos+1));
    vec[pos] = item;
}


template< typename T>
void insert_sorted(T *arr, const T &item, int size){
    T *upper = std::upper_bound(arr, arr + size, item);
    int pos = upper - arr;
    std::move_backward(upper, upper+(size-pos), upper+(size-pos+1));
    arr[pos] = item;
}


template<typename T>
std::string join(const T &v, char separator, int max_size){

    if(max_size == 0 || v.empty()) return "";

    typename T::const_iterator it = v.cbegin();
    std::string key = patch::to_string(*(it++));

    for(int i = 1; it != v.cend() && i < max_size; i++, it++){
        key += (separator + patch::to_string(*it));
    }

    return key;
}


template<typename T>
std::string join(T *v, char separator, int max_size){

    std::string key = patch::to_string(*(v++));

    for(int i = 1; i < max_size; i++, v++){
        key += (separator + patch::to_string(*v));
    }

    return key;
}


template<typename T>
std::string join(const T &v, char separator){

    if(v.empty()) return "";

    typename T::const_iterator it = v.cbegin();
    std::string key = patch::to_string(*(it++));

    for(; it != v.cend(); it++){
        key += (separator + patch::to_string(*it));
    }

    return key;
}


/*
    Obtained from https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
    Author: Evan Teran
 */
template<typename T>
void _split(const std::string &s, char delim, T result) {
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
    _split(s, delim, std::back_inserter(elems));
    return elems;
}


template<typename T>
void split(const std::string &s, char delim, T *target) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    int i = 0;
    T item_type;
    while (std::getline(ss, item, delim)) {
        std::istringstream(item) >> item_type;
        target[i++] = item_type;
    }
}


#endif
