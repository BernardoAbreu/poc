#include "node.h"
#include "util.h"


// Node::Node(){
//     std::cout << "shit" << std::endl; 
// }


Node::Node(const Pattern &pat, int size){
    // std::cout << size << std::endl; 
    this->children = std::vector<bool>(size);
    // std::cout << this->children.size() << std::endl;
    this->pat = pat;
}


// Node::Node(const Node& other){
//     this->children = other.children;
//     // std::cout << "Shit " << this->children.size() << std::endl;
//     this->pat = other.pat;
// }

// // copy assignment
// Node& Node::operator=(const Node& other){
//     if (this == &other) {
//         return *this;
//     }
//     this->children = other.children;
//     // std::cout << "Shit 2 " << this->children.size() << std::endl;
//     this->pat = other.pat;
//     return *this;
// }


std::ostream& operator<<(std::ostream& out, const Node& node){
    out << node.pat;

    for(auto& child: node.next){
        out << '>';
        out << child->pat.molecules[0];
        for(int i = 1; i < child->pat.get_mol_size(); i++){
            out << ',' << child->pat.molecules[i];
        }
    }

    return out;
}
