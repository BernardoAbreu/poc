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
    unsigned int count;     // Count of pointers

    InterPattern(int index);

	inline void add_col(unsigned int c) { this->cols.push_front(c); }

    // Was already checked during quality update
    inline bool is_visited() const { return this->flag & 0b01; }

    // Is the top pattern for at least a column, but not in all of them
    inline bool is_invalid() const { return (this->flag >> 1) & 0b01; }

    // Tied in quality with another pattern
    inline bool is_tied() const { return (this->flag >> 2) & 0b01; }

    inline void set_visited() { this->flag |= 0b01; }
    inline void reset_visited() { this->flag &= (~0b01); }

    inline void set_invalid() { this->flag |= 0b10; }

    inline void set_tied() { this->flag |= 0b100; }

    inline bool operator<(const InterPattern &other) const { return this->quality < other.quality; }

    friend std::ostream& operator<<(std::ostream& out, const InterPattern& pat);

};

#endif