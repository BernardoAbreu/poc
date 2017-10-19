#include "pattern.h"

Pattern::Pattern(std::string s){
	mol_set = s;
	quality = best_quality = gap = 0;
}


template <class InputIterator>
Pattern::Pattern(std::string s, InputIterator first, InputIterator last){
	mol_set = s;
	molecules = std::vector<int>(first, last);
	quality = best_quality = gap = 0;
}

Pattern::Pattern(std::string s, int m){
	mol_set = s;
	quality = best_quality = gap = m;
}


template <class InputIterator>
Pattern::Pattern(std::string s, int m, InputIterator first, InputIterator last){
	mol_set = s;
	molecules = std::vector<int>(first, last);
	quality = best_quality = gap = m;
}


void Pattern::set_quality(double q){
	quality = q;
}


void Pattern::set_gap(double g){
	gap = g;
}


void Pattern::set_string(std::string s){
	mol_set = s;
}


void Pattern::add_molecule(int m){
	this.molecules.push_back(m);
}


void Pattern::add_point(int p){
	this.points.push_back(p);
}


double Pattern::get_quality(){
	return this.quality;
}


double Pattern::get_best_quality(){
	return this.best_quality;
}


double Pattern::get_gap(){
	return this.gap;
}

int main(){
	return 0;
}