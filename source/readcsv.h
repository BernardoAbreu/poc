#ifndef READCSV_H
#define READCSV_H

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "graph.h"
#include "util.cpp"

template<typename T>
using Matrix = std::vector<std::vector<T> >;


template<typename T>
Matrix<T> read_file_to_vector(std::string input_file, char delim);

bool mol_cmp_function (std::pair<int, double> i, std::pair<int, double> j);

void build_matrix_from_csv(std::string input_file,
						   Matrix<std::pair<int, double> > *points);

void build_matrix_from_csv(std::string input_file,
						   Matrix<std::pair<int, double> > *points,
						   bool max);

#endif