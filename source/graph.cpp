#include "graph.h"
#include "util.h"

Node* Graph::insert(int level, Node &n){
    this->level[level].push_front(Node(n));
    return &(this->level[level].front());
}


std::ostream& operator<<(std::ostream& out, const Graph& g){
    int i = 0;

    for(auto& level: g.level){
        if(level.empty()){
            break;
        }
        out << "Level " << i++ << ':';
        for(auto& node : level){
            out << ' ' << node;
        }
        out << std::endl;
    }

    return out;
}
