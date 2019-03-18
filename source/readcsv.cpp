#include "readcsv.h"


template<typename T>
void __read_file(std::istream& myfile, std::vector<std::vector<T> > &v, char delim){

    std::string line;

    while (getline(myfile, line)){
        v.push_back(split<std::vector<T> >(line, delim));
    }

}


template<typename T>
void read_file_to_vector(std::string input_file, char delim, std::vector<std::vector<T> > &v){

    if(input_file == ""){
        __read_file(std::cin, v, delim);
    }
    else{
        std::ifstream myfile(input_file.c_str());

        if (myfile.is_open()){
             __read_file(myfile, v, delim);
            myfile.close();
        }
        else{
            std::cerr << "Unable to open file"; 
        }
    }
}


bool mol_cmp(std::pair<int, double> i, std::pair<int, double> j) {
    if(i.second == j.second){
        return i.first > j.first;
    }
    else return (i.second > j.second);
}


bool mol_cmp_reverse(std::pair<int, double> i, std::pair<int, double> j) {
    if(i.second == j.second){
        return i.first < j.first;
    }
    else return (i.second < j.second);
}


std::pair<unsigned int, unsigned int> build_matrix_from_csv(std::string input_file, 
                                          std::pair<int, double> **&matrix,
                                          bool min){

    std::vector<std::vector<double> > v;
    read_file_to_vector<double>(input_file, ' ', v);

    unsigned int max_points = v.size();
    unsigned int mol_size = v[0].size();

    matrix = new std::pair<int, double>*[max_points];

    for(unsigned int i = 0; i < max_points; i++){
        std::vector<std::pair<int, double> > v_mol;
        matrix[i] = new std::pair<int, double>[mol_size];

        for(unsigned int j = 0; j < mol_size; j++){
            matrix[i][j] = std::make_pair(j, v[i][j]);
        }
        std::sort(matrix[i], matrix[i] + mol_size, 
                  (min) ? mol_cmp_reverse : mol_cmp);

    }
    std::cout << matrix[0][0].first << std::endl;
    return std::make_pair(max_points, mol_size);
}

std::pair<unsigned int, unsigned int> build_matrix_from_csv(std::string input_file,
                                          std::pair<int, double> **&points){
    return build_matrix_from_csv(input_file, points, true);
}
