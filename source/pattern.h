#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <forward_list>
#include <ostream>


struct Pattern{
    double quality, best_quality;
    int mol_size;
    int *molecules;
    std::forward_list<int> points;

    Pattern();

    Pattern(double limit, int mols[], int size);
    
    Pattern(const Pattern& other);

    Pattern& operator=(const Pattern& other);

    ~Pattern();

    inline void add_point(int p) { this->points.push_front(p); }

    inline void add_gap(double gap){
        this->best_quality = this->quality += gap;
    }

    inline double get_quality() { return this->quality; }

    inline double get_best_quality() { return this->best_quality; }

    inline int get_mol_size() const { return this->mol_size; }

    friend std::ostream& operator<<(std::ostream& out, const Pattern& p);

};


#endif