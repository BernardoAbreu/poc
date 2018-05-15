#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <list>

#include <math.h>
#include <algorithm>
#include <string>
#include <set>
#include <unordered_map>

#include "graph.h"

typedef std::pair<int, double> mol_info;

typedef std::unordered_map<std::string, Node*> HashMolMap;


// void level_traverse(Graph *g);

void build_graph(Graph *g, const std::vector<std::vector<mol_info> > &points, int min_group_size);


void level1(Graph &g, std::list<Pattern> &sel);
void level1(Graph &g, std::list<Pattern> &sel, bool);

void post_process(std::list<Pattern> &selected);
void post_process(std::list<Pattern> &selected, bool);

#endif


