#ifndef NODE_H
#define NODE_H

#include <list>
#include <vector>
#include "pattern.h"


struct Node{
    Pattern pat;
    std::list<Node*> next;
    std::vector<bool> children;

    Node(const Pattern&, int);

    inline void add_child(int mol, Node* np){
        if(!this->children[mol]){
            this->children[mol] = true;
            this->next.push_back(np);
        }
    }
};

#endif