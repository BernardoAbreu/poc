#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <list>

#include <cmath>
#include <algorithm>
#include <string>
#include <unordered_map>

#include <fstream>

#include "graph.h"
#include "util.h"


typedef std::pair<int, double> index_value;

typedef std::unordered_map<std::string, Node*> HashMolMap;

void build_graph(Graph &g, index_value **points,
				 const std::pair<unsigned int, unsigned int> &dimensions,
				 int min_group_size);

void build_graph_from_file(Graph &graph, std::string filename, int min_group_size);

void level1(Graph &g, std::list<Pattern> &sel);

void post_process(std::list<Pattern> &selected);

#endif
