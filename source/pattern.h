#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <forward_list>
#include <ostream>


struct Pattern{
    double quality;
    std::vector<unsigned int> rows;
    std::vector<unsigned int> cols;

    Pattern();
    Pattern(double quality, std::forward_list<unsigned int> &points);

    friend std::ostream& operator<<(std::ostream& out, const Pattern& p);

};

#endif