#ifndef READCSV_H
#define READCSV_H

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "util.h"


template<typename T>
void read_file_to_vector(std::string input_file, char delim, std::vector<std::vector<T> >&);

bool mol_cmp_function (std::pair<int, double> i, std::pair<int, double> j);

std::pair<unsigned int, unsigned int> build_matrix_from_csv(std::string input_file,
						   std::pair<int, double> **&points);

std::pair<unsigned int, unsigned int> build_matrix_from_csv(std::string input_file,
						   std::pair<int, double> **&points,
						   bool max);

#endif