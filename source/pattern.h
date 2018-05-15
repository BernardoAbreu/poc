#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <string>
#include <list>
#include <string>

struct Pattern{
    double quality, best_quality, gap, limit;
    std::string str;
    std::vector<int> molecules;
    std::list<int> points;

    Pattern();

    Pattern(std::string, double limit, const std::vector<int> &molecules, int size);

    inline void add_point(int p) { this->points.push_back(p); }

    inline void add_gap(double gap){
        this->quality += (gap * gap);
        this->gap += gap;
    }

    inline double get_quality() { return this->quality; }

    inline double get_best_quality() { return this->best_quality; }

    inline double get_gap() { return this->gap; }

    inline double get_limit() { return this->limit; }

    inline std::string get_string() { return this->str; }

};


#endif