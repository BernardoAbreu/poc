#ifndef PROCESS_H
#define PROCESS_H

#include <list>

#include <cmath>
#include <algorithm>
#include <unordered_map>

#include <fstream>

#include "graph.h"
#include "util.h"
#include "pattern.h"


typedef std::pair<unsigned short int, double> index_value;

typedef std::unordered_map<std::string, Node*> HashMolMap;


void build_graph(Graph &g, index_value **matrix,
				 const std::pair<unsigned int, unsigned int> &dimensions,
				 int min_group_size);

void build_graph(Graph &graph, const std::string &filename, int min_group_size);

void build_graph_from_file(Graph &graph, const std::string &filename, int min_group_size);

void level1(Graph &g, std::list<std::pair<int, Pattern> > &sel);

void post_process(std::list<Pattern> &selected);

#endif
