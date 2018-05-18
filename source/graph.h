#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <list>
#include <forward_list>
#include <ostream>
#include <iterator>

#include "pattern.h"
#include "node.h"

// struct Pattern{
//     double quality, best_quality, gap, limit;
//     std::string str;
//     std::vector<int> molecules;
//     std::list<int> points;
// };

// struct Node{
//     Pattern pat;
//     std::list<Node*> next;
//     std::vector<bool> children;
// };



class Graph{

public:
    std::vector<std::forward_list<Node> > level;
    // void level_traverse(std::ostream&);

    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};


#endif