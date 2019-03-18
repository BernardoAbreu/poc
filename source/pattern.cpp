#include "pattern.h"
#include "util.h"

Pattern::Pattern(){
    this->quality = this->best_quality = 0;
}


Pattern::Pattern(double limit, const std::vector<int> &mols, int size){
    this->quality = this->best_quality = 0;
    this->molecules = std::vector<int>(mols.begin(), mols.begin() + size);
}

std::ostream& operator<<(std::ostream& out, const Pattern& pat){
    out << ' ' << pat.molecules[0];
    // int mol_size = pat.get_mol_size();
    int mol_size = pat.molecules.size();
    for(int i = 1; i < mol_size ; i++){
        out << ',' << pat.molecules[i];
    }

    out << '(' << pat.quality << ',' << pat.best_quality << ')';
    out << '[' << join(pat.points, ',') << ']';

    return out;
}
