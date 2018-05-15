#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <list>
#include <ostream>
#include <iterator>


struct Pattern{
    double quality, best_quality, gap, limit;
    std::string mol_set;
    std::vector<int> molecules;
    std::list<int> points;
};

struct Node{
    Pattern pat;
    std::list<Node*> next;
    std::vector<long> children;
};



class Graph{

public:
    std::vector<std::list<Node> > level;
    // void level_traverse(std::ostream&);

    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};


#endif