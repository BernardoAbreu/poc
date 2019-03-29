#include "inter_pattern.h"


InterPattern::InterPattern(int index){
    this->quality = 0;
    this->flag = 0;
    this->row_index = index;
    this->count = 0;
}


std::ostream& operator<<(std::ostream& out, const InterPattern& pat){
    out << '(' << (pat.is_invalid() ? 'x' : 'o') << ' ' << pat.count << ' ' << pat.is_visited() << ')';
    out << '{' << pat.row_index << '}';
    out << '[';
    if(!pat.cols.empty()){
        auto it = pat.cols.begin();
        out << (*it);
        it++;
        for(; it != pat.cols.end(); it++){
            out << ',' << (*it);
        }
    }
    out << ']';
    out << '(' << pat.quality << ')';

    return out;
}