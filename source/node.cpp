#include "node.h"
#include "util.h"

Node::Node(const Pattern &pat, int size){
    this->children = std::vector<bool>(size);
    this->pat = pat;
}


std::ostream& operator<<(std::ostream& out, const Node& node){
    out << node.pat;

    for(auto& child: node.next){
        out << '>';
        out << ' ' << child->pat.molecules[0];
        for(int i = 1; i < child->pat.get_mol_size(); i++){
            out << ',' << child->pat.molecules[i];
        }
    }

    out << '[' << join(node.pat.points, ',') << ']';

    return out;
}
