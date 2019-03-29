#ifndef PROCESS_H
#define PROCESS_H

#include <list>

#include <cmath>
#include <algorithm>
#include <unordered_map>

#include <fstream>

#include "util.h"
#include "pattern.h"
#include "inter_pattern.h"


typedef std::pair<unsigned short int, double> index_value;

typedef std::unordered_map<std::string, InterPattern*> HashMolMap;


void extract_patterns(const std::string &filename, int min_group_size,
					  std::list<Pattern> &out);


// void build_rows(const std::string &filename, int min_group_size,
//                 std::list<std::pair<int, Pattern>> &out_aux,
//                 std::list<Pattern> &out);

void post_process(std::list<Pattern> &selected);

#endif
