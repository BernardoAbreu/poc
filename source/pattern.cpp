#include "pattern.h"
#include "util.h"


Pattern::Pattern(){
    this->quality = 0;
    this->tied = false;
}


Pattern::Pattern(double quality, std::forward_list<unsigned int> &points){
    this->quality = quality;
    this->cols = std::vector<unsigned int>{std::make_move_iterator(std::begin(points)),
                                           std::make_move_iterator(std::end(points))};
    this->tied = false;
}

Pattern::Pattern(double quality, std::forward_list<unsigned int> &points, bool tied){
    this->quality = quality;
    this->cols = std::vector<unsigned int>{std::make_move_iterator(std::begin(points)), 
                                  std::make_move_iterator(std::end(points)) };
    this->tied = tied;
}


std::ostream& operator<<(std::ostream& out, const Pattern& pat){
    out << '[' << join(pat.rows, ',') << ']';
    out << '[' << join(pat.cols, ',') << ']';
    out << '(' << pat.quality << ')';

    return out;
}