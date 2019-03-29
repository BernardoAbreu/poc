#include "pattern.h"
#include "util.h"


Pattern::Pattern(){
    this->quality = 0;
}


Pattern::Pattern(double quality, std::forward_list<unsigned short int> &points){
    this->quality = quality;
    this->cols = std::vector<unsigned short int>{std::make_move_iterator(std::begin(points)), 
                                  std::make_move_iterator(std::end(points)) };
}


std::ostream& operator<<(std::ostream& out, const Pattern& pat){
    out << '[' << join(pat.rows, ',') << ']';
    out << '[' << join(pat.cols, ',') << ']';
    out << '(' << pat.quality << ')';

    return out;
}