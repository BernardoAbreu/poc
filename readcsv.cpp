#include <sstream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <algorithm>

#include "util.cpp"
#include "graph.h"

using namespace std;


template<typename T>
vector<std::vector<T> > read_file_to_vector(string input_file, char delim){
    string line;

    vector<std::vector<T> > v;

    ifstream myfile(input_file.c_str());

    if (myfile.is_open()){
        while ( getline(myfile, line)){
            v.push_back(split<vector<T> >(line, delim));
        }

        myfile.close();
    }
    else{
        cout << "Unable to open file"; 
    }
    return v;
}


bool mol_cmp_function (mol_info i,mol_info j) {
    if(i.second == j.second){
        return i.first > j.first;
    }
    else return (i.second > j.second);
}


void build_matrix_from_csv(string input_file, vector<vector<mol_info> > *points){

    vector<vector <double> > v = read_file_to_vector<double>(input_file, ' ');

    int max_points = v.size();

    for(int i = 0; i < max_points; i++){
        std::vector<mol_info> v_mol;

        for(unsigned int j = 0; j < v[i].size(); j++){
            v_mol.push_back(make_pair(j,v[i][j]));
        }
        sort(v_mol.begin(), v_mol.end(), mol_cmp_function);

        points->push_back(v_mol);

    }
}


// int main( int argc, char * args[] ){

//     string input_file = "C_X_TP_t10_T.txt";
//     vector<vector<mol_info> > points;

//     build_matrix_from_csv(input_file, &points);

//     for(int i = 0; i < points.size(); i++){
//         for(int j = 0; j < points[i].size(); j++){
//             cout << points[i][j].first << ',' << points[i][j].second << std::scientific << '\t';
//         }
//         cout << endl;
//     }

//     return 0;
// }