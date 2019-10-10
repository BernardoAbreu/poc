#ifndef INTER_PATTERN_H
#define INTER_PATTERN_H

#include <vector>
#include <forward_list>
#include <ostream>
#include "util.h"


struct InterPattern{
    double quality;
    unsigned int row_index;
    unsigned char flag;
    std::forward_list<unsigned int> cols;
    unsigned int count;

    InterPattern(int index);

	inline void add_col(unsigned int c) { this->cols.push_front(c); }

    inline bool is_visited() const { return this->flag & 0b01; }

    inline bool is_invalid() const { return (this->flag >> 1) & 0b01; }

    inline void set_visited() { this->flag |= 0b01; }
    inline void reset_visited() { this->flag &= 0b10; }

    inline void set_invalid() { this->flag |= 0b10; }

    inline bool operator<(const InterPattern &other) const { return this->quality < other.quality; }

    friend std::ostream& operator<<(std::ostream& out, const InterPattern& pat);

};

#endif