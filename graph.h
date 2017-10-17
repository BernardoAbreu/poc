#ifndef GRAPH_H
#define GRAPH_H


#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>


struct pattern{
    float quality, best_quality, gap;
    std::string mol_set;
    std::vector<int> molecules;
    std::list<int> points;
};

struct node{
    pattern pat;
    std::list<node*> next;
    std::vector<long> children;
};
 
struct graph{
    std::vector<std::list<node> > level;
};

typedef std::pair<int, double> mol_info;


typedef std::map<std::string, node*> MolMap;

typedef std::unordered_map<std::string, node*> HashMolMap;


void level_traverse(graph *g);

void build_graph(graph *g, std::vector<std::vector<mol_info> > *points, int min_group_size);

#endif