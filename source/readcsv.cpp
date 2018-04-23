#include "readcsv.h"

using namespace std;


template<typename T>
vector<std::vector<T> > read_file_to_vector(string input_file, char delim){
    string line;

    vector<std::vector<T> > v;

    ifstream myfile(input_file.c_str());

    if (myfile.is_open()){
        while (getline(myfile, line)){
            v.push_back(split<vector<T> >(line, delim));
        }

        myfile.close();
    }
    else{
        cout << "Unable to open file"; 
    }
    return v;
}


bool mol_cmp(std::pair<int, double> i,std::pair<int, double> j) {
    if(i.second == j.second){
        return i.first > j.first;
    }
    else return (i.second > j.second);
}


bool mol_cmp_reverse(std::pair<int, double> i,std::pair<int, double> j) {
    if(i.second == j.second){
        return i.first < j.first;
    }
    else return (i.second < j.second);
}


void build_matrix_from_csv(string input_file, vector<vector<std::pair<int, double> > > *points){

    vector<vector <double> > v = read_file_to_vector<double>(input_file, ' ');

    int max_points = v.size();

    for(int i = 0; i < max_points; i++){
        std::vector<std::pair<int, double> > v_mol;

        for(unsigned int j = 0; j < v[i].size(); j++){
            v_mol.push_back(make_pair(j,v[i][j]));
        }
        sort(v_mol.begin(), v_mol.end(), mol_cmp);

        points->push_back(v_mol);

    }
}
