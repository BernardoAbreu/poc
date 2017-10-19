#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <string>
#include <list>

class Pattern{
private:
    double quality, best_quality, gap;
    std::string mol_set;
    std::vector<int> molecules;
    std::list<int> points;

public:
	void set_quality(double);

	void set_gap(double);

	void set_string(std::string);

	void add_molecule(int);

	void add_point(int);

	double get_quality();

	double get_best_quality();

	double get_gap();
};


#endif