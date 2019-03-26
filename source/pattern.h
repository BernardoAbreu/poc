#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <forward_list>
#include <ostream>
#include "util.h"


struct Pattern{
    float quality;
    std::vector<unsigned short int> rows;
    std::vector<unsigned short int> cols;

	Pattern();
    Pattern(float quality, std::forward_list<unsigned short int> &points);

    friend std::ostream& operator<<(std::ostream& out, const Pattern& p);

};

#endif