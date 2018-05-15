#ifndef NODE_H
#define NODE_H

#include "pattern.h"


struct Node{
    Pattern pat;
    std::list<Node*> next;
    std::vector<bool> children;

    Node(int, const Pattern&);

    inline void add_child(int mol, Node* np){
        if(!this->children[mol]){
            this->children[mol] = true;
            this->next.push_back(np);
        }
    }
};

#endif