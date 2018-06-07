#include "pattern.h"


Pattern::Pattern(){
    this->quality = this->best_quality = 0;
}


Pattern::Pattern(double limit, const std::vector<int> &mols, int size){
    this->quality = this->best_quality = 0;
    this->molecules = std::vector<int>(mols.begin(), mols.begin() + size);
}