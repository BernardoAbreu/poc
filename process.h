#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <list>
#include "graph.h"

void level1(graph *g, std::vector<pattern> *sel);

void post_process(std::vector<pattern> *selected, std::list<pattern> *out);

#endif


