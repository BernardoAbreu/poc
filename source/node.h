#ifndef NODE_H
#define NODE_H

#include <forward_list>
#include <vector>
#include <iostream>


struct Node{
    float quality, best_quality;
    std::forward_list<unsigned short int> points;
    std::forward_list<Node*> next;
    std::vector<bool> children;

    Node(int);

    inline void add_child(int mol, Node* np){
        if(!this->children[mol]){
            this->children[mol] = true;
            this->next.push_front(np);
        }
    }

    inline void add_point(unsigned short int p) { this->points.push_front(p); }

    inline void add_gap(double gap){
        this->best_quality = this->quality += gap;
    }

    friend std::ostream& operator<<(std::ostream& out, const Node& n);
};

#endif