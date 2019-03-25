#ifndef NODE_H
#define NODE_H

#include <forward_list>
#include <vector>
#include "pattern.h"
#include <iostream>

struct Node{
    Pattern pat;
    std::forward_list<Node*> next;
    std::vector<bool> children;

    // Node();
    Node(int);
    // Node(const Pattern&, int);
    // Node(const Node& other);
    // Node& operator=(const Node& other);

    inline void add_child(int mol, Node* np){
        // std::cout << this->children.size() << ' ' << mol << std::endl;
        if(!this->children[mol]){
            this->children[mol] = true;
            this->next.push_front(np);
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Node& n);
};

#endif