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

typedef std::unordered_map<std::string, node*> HashMolMap;


void level_traverse(Graph *g);

void build_graph(Graph *g, std::vector<std::vector<mol_info> > *points, int min_group_size);


void level1(Graph *g, std::vector<pattern> *sel);
void level1(Graph *g, std::vector<pattern> *sel, bool);

void post_process(std::vector<pattern> *selected, std::list<pattern> *out);
void post_process(std::vector<pattern>*, std::list<pattern>*, bool);

#endif


