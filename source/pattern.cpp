#include "pattern.h"
#include "util.h"
#include <iostream>

Pattern::Pattern(){
    this->quality = this->best_quality = 0;
    this->mol_size = -1;
    // std::cout << "I'm alive\n";
}


Pattern::Pattern(double limit, int mols[], int size){
    this->quality = this->best_quality = 0;
    this->mol_size = size;
    this->molecules = new int[size];
    std::copy(mols, mols+size, this->molecules);
}

Pattern::Pattern(const Pattern& other){
    this->quality = other.quality;
    this->best_quality = other.best_quality;
    this->mol_size = other.mol_size;
    if(other.mol_size > -1){
        this->molecules = new int[other.mol_size];
        std::copy(other.molecules, other.molecules+this->mol_size, this->molecules);
        this->points = other.points;
    }
}


// copy assignment
Pattern& Pattern::operator=(const Pattern& other){
    if (this == &other) {
        return *this;
    }

    if(this->mol_size > -1){
        delete[] this->molecules;
    }

    this->quality = other.quality;
    this->best_quality = other.best_quality;
    this->mol_size = other.mol_size;
    if(other.mol_size > -1){
        this->molecules = new int[other.mol_size];
        std::copy(other.molecules, other.molecules+this->mol_size, this->molecules);
        this->points = other.points;
    }
    return *this;
}

Pattern::~Pattern(){
    if(this->mol_size > -1){
        delete[] this->molecules;
    }
}

std::ostream& operator<<(std::ostream& out, const Pattern& pat){
    out << ' ' << pat.molecules[0];
    int mol_size = pat.get_mol_size();
    // int mol_size = pat.molecules.size();
    for(int i = 1; i < mol_size ; i++){
        out << ',' << pat.molecules[i];
    }

    out << '(' << pat.quality << ',' << pat.best_quality << ')';
    out << '[' << join(pat.points, ',') << ']';

    return out;
}
