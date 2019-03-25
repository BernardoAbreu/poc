#include "pattern.h"
#include "util.h"
#include <iostream>

Pattern::Pattern(){
    this->quality = this->best_quality = 0;
    // this->mol_size = -1;
}


std::ostream& operator<<(std::ostream& out, const Pattern& pat){
    // int mol_size = pat.get_mol_size();

    // if(mol_size > 0){
    //     out << pat.molecules[0];
    //     for(int i = 1; i < mol_size ; i++){
    //         out << ',' << pat.molecules[i];
    //     }
    // }

    out << '(' << pat.quality << ',' << pat.best_quality << ')';
    out << '[' << join(pat.points, ',') << ']';

    return out;
}
