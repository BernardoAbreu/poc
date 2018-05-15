#include "pattern.h"


Pattern::Pattern(){
    this->str = "";
    this->limit = this->quality = this->best_quality = this->gap = 0;
}

Pattern::Pattern(std::string s, double limit, const std::vector<int> &mols, int size){
    this->str = s;
    this->limit = limit;
    this->quality = this->best_quality = this->gap = 0;
    this->molecules = std::vector<int>(mols.begin(), mols.begin() + size);
}