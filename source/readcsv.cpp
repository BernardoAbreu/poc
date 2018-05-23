#include "readcsv.h"


template<typename T>
Matrix<T> read_file_to_vector(std::string input_file, char delim){
    std::string line;

    Matrix<T> v;

    std::ifstream myfile(input_file.c_str());

    if (myfile.is_open()){
        while (getline(myfile, line)){
            v.push_back(split<std::vector<T> >(line, delim));
        }

        myfile.close();
    }
    else{
        std::cerr << "Unable to open file"; 
    }
    return v;
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


void build_matrix_from_csv(std::string input_file,
                           Matrix<std::pair<int, double> > *points,
                           bool max){

    Matrix<double> v = read_file_to_vector<double>(input_file, ' ');

    int max_points = v.size();

    for(int i = 0; i < max_points; i++){
        std::vector<std::pair<int, double> > v_mol;

        for(unsigned int j = 0; j < v[i].size(); j++){
            v_mol.push_back(std::make_pair(j,v[i][j]));
        }
        std::sort(v_mol.begin(), v_mol.end(), 
            (max) ? mol_cmp : mol_cmp_reverse);

        points->push_back(v_mol);

    }
}

void build_matrix_from_csv(std::string input_file,
                           Matrix<std::pair<int, double> > *points){
    build_matrix_from_csv(input_file, points, true);
}
