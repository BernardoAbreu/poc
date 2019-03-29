#include "node.h"
#include "util.h"


Node::Node(int size){
    this->visited = false;
    this->quality = this->best_quality = 0;
    this->children = std::vector<bool>(size);
}


std::ostream& operator<<(std::ostream& out, const Node& node){
    if(!node.points.empty()){
        auto it = node.points.begin();
        out << (*it);
        for(; it != node.points.end(); it++){
            out << ',' << (*it);
        }
    }

    return out;
}
