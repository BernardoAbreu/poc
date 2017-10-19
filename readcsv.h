#ifndef READCSV_H
#define READCSV_H

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "graph.h"
#include "util.cpp"

using namespace std;


template<typename T>
vector<std::vector<T> > read_file_to_vector(string input_file, char delim);

bool mol_cmp_function (std::pair<int, double> i,std::pair<int, double> j);

void build_matrix_from_csv(string input_file, vector<vector<std::pair<int, double> > > *points);

#endif