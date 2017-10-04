#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "util.cpp"

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


int main( int argc, char * args[] ){

    string input_file = "data.csv";
   
    vector<vector <int> > v = read_file_to_vector<int>(input_file, '\t');

    for(int i = 0; i < v.size(); i++){
        for(int j = 0; j < v[i].size(); j++){
            cout << v[i][j] << ' ';
        }
        cout << endl;
    }

    return 0;
}