#include "pattern.h"
#include "util.h"
#include <iostream>

Pattern::Pattern(){
    this->quality = this->best_quality = 0;
    this->mol_size = -1;
}


Pattern::Pattern(int mols[], int size){
    this->quality = this->best_quality = 0;
    this->mol_size = size;
    this->molecules = new int[size];
    std::copy(mols, mols+size, this->molecules);
}


Pattern::Pattern(int new_mol){
    this->quality = this->best_quality = 0;
    this->mol_size = 1;
    this->molecules = new int[1];
    this->molecules[0] = new_mol;
}


Pattern::Pattern(const Pattern& other, int new_mol){
    // std::cout << other.mol_size << std::endl;
    bool new_one = false;
    int i;
    this->quality = this->best_quality = 0;
    this->mol_size = other.mol_size + 1;

    this->molecules = new int[this->mol_size];
    for(i = 0; i < other.mol_size; i++){
        if(new_mol < other.molecules[i]){
            this->molecules[i] = new_mol;
            new_one = true;
            break;
        }
        this->molecules[i] = other.molecules[i];
    }

    if(new_one){
        for(;i < other.mol_size; i++){
            this->molecules[i+1] = other.molecules[i];
        }
    }
    else{
        this->molecules[i] = new_mol;
    }
}


Pattern::Pattern(const Pattern& other){
    this->quality = other.quality;
    this->best_quality = other.best_quality;
    this->mol_size = other.mol_size;
    // std::cout << "other.mol_size: " << other.mol_size << std::endl;
    // std::cout << other.mol_size * sizeof(int) << std::endl;
    if(other.mol_size > -1){
        this->molecules = new int[other.mol_size];
        // std::cout << "Hello\n";
        std::copy(other.molecules, other.molecules + this->mol_size, this->molecules);
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
    int mol_size = pat.get_mol_size();

    if(mol_size > 0){
        out << pat.molecules[0];
        for(int i = 1; i < mol_size ; i++){
            out << ',' << pat.molecules[i];
        }
    }

    out << '(' << pat.quality << ',' << pat.best_quality << ')';
    out << '[' << join(pat.points, ',') << ']';

    return out;
}
