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


void build_graph(const std::string &filename, int min_group_size, std::list<std::pair<int, Pattern>>& out);


// void level1(Graph &g, std::list<std::pair<int, Pattern> > &sel);

void post_process(std::list<Pattern> &selected);

#endif
