#include <sstream>
#include <fstream>
#include <iterator>
#include <string>


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}


template<typename T>
std::string join(T v, char separator, int max_size){
    typename T::iterator it;

    if(max_size == 0 || v.size() == 0) return "";

    std::string key = patch::to_string(*(v.begin()));

    int i = 1;
    for(it=v.begin()+1; it != v.end() && i < max_size; i++, it++){
        key += (separator + patch::to_string(*it));
    }

    return key;
}

template<typename T>
std::string join(T v, char separator){
    return join(v, separator, v.size());
}


// template<typename T>
// std::string join(std::vector<T> v, char separator, int size, int remove_pos){
//     std::string key = "";
//     int k;
//     if(v.size() > 1 && size){
//         if(remove_pos == 0){
//             key = patch::to_string(v[1]);
//             k = 2;
//         }
//         else{
//             key = patch::to_string(v[0]);
//             k = 1;
//         }    
//     }

//     for(; k < size + 1; k++){
//         if(k != remove_pos){
//             key += (separator + patch::to_string(v[k]));
//         }
//     }
//     return key;
// }

template<typename T>
std::string join(T v, char separator, int max_size, int remove_pos){
    typename T::iterator it = v.begin();
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

    for(; it != v.end() && k < (max_size + 1); k++, it++){
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