#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
// #include <list>
#include <forward_list>
#include <ostream>
#include <iterator>

#include "pattern.h"
#include "node.h"

class Graph{

public:
    std::vector<std::forward_list<Node> > level;
    // std::vector<std::list<Node> > level;

    Node* insert(int level, Node &n);

    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};


#endif
