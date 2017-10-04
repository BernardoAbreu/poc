#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <algorithm>
#include <iomanip>

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


bool myfunction (mol_info i,mol_info j) {
    if(i.second == j.second){
        return i.first > j.first;
    }
    else return (i.second > j.second);
}


int main( int argc, char * args[] ){

    string input_file = "C_X_TP_t10_T.txt";

    vector<vector <double> > v = read_file_to_vector<double>(input_file, ' ');

    // for(int i = 0; i < v.size(); i++){
    //     cout << v[i].size() << ':';
    //     for(int j = 0; j < v[i].size(); j++){
    //         cout << v[i][j] << ',';
    //     }
    //     cout << endl;
    // }

    int max_points = v.size();
    vector<vector<mol_info> > points;
    double number;
    for(int i = 0; i < max_points; i++){
        std::vector<mol_info> v_mol;

        for(int j = 0; j < v[i].size(); j++){
            // std::istringstream(v[i][j]) >> std::setprecision(100) >> number;
            v_mol.push_back(make_pair(j,v[i][j]));
        }
        sort(v_mol.begin(), v_mol.end(), myfunction);

        points.push_back(v_mol);

    }

    for(int i = 0; i < points.size(); i++){
        cout << points[i].size() << ':';
        for(int j = 0; j < points[i].size(); j++){
            cout << points[i][j].first << ',' << points[i][j].second << '\t';
        }
        cout << endl;
    }

    return 0;
}