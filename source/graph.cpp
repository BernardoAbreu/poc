#include "graph.h"
#include "util.h"


Node* Graph::insert(int level, Node &n){
    this->level[level].push_front(n);

    return &(this->level[level].front());
}


std::ostream& operator<<(std::ostream& out, const Graph& g){
    int i = 0;

    for(auto& level: g.level){
        out << "Level " << i++ << ':';
        for(auto& node : level){
            out << ' ' << join(node.pat.molecules, ',')
                << '(' << node.pat.quality << ',' << node.pat.best_quality << ')';

            for(auto& child: node.next){
                out << '>' << join(child->pat.molecules, ',');
            }

            out << '[' << join(node.pat.points, ',') << ']';
        }
        out << std::endl;
    }
    return out;
}
