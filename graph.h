#ifndef GRAPH_H
#define GRAPH_H


// #include <math.h>
// #include <algorithm>
#include <vector>
#include <string>
#include <list>
#include <ostream>
#include <iterator>
// #include <set>
// #include <map>
// #include <unordered_map>
// #include "util.cpp"
// #include "util.h"


struct pattern{
    double quality, best_quality, gap;
    std::string mol_set;
    std::vector<int> molecules;
    std::list<int> points;
};

struct node{
    pattern pat;
    std::list<node*> next;
    std::vector<long> children;
};



class Graph{

public:
    std::vector<std::list<node> > level;
    void level_traverse(std::ostream&);

    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};


#endif