#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <list>
#include "graph.h"

void level1(graph *g, std::vector<node> *sel);

void post_process(std::vector<node> *selected, std::list<node> *out);

#endif


