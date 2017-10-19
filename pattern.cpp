#include "pattern.h"

Pattern::Pattern(string s){
	mol_set = s;
	quality = best_quality = gap = 0;
}


template <class InputIterator>
Pattern::Pattern(string s, InputIterator first, InputIterator last){
	mol_set = s;
	molecules = vector<int>(first, last);
	quality = best_quality = gap = 0;
}

Pattern::Pattern(string s, int m){
	mol_set = s;
	quality = best_quality = gap = m;
}


template <class InputIterator>
Pattern::Pattern(string s, int m. InputIterator first, InputIterator last){
	mol_set = s;
	molecules = vector<int>(first, last);
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


void Pattern::add_molecule(int){

}


void Pattern::add_point(int){

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