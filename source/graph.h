#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <forward_list>
#include <ostream>
#include <iterator>

#include "pattern.h"
#include "node.h"

class Graph{

public:
    std::vector<std::forward_list<Node> > level;

    // Node* insert(int level, Node &n);
    Node* insert(int level, int n);

    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};


#endif
