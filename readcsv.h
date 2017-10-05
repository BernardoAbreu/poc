#ifndef READCSV_H
#define READCSV_H

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
vector<std::vector<T> > read_file_to_vector(string input_file, char delim);

bool mol_cmp_function (mol_info i,mol_info j);

void build_matrix_from_csv(string input_file, vector<vector<mol_info> > *points);

#endif