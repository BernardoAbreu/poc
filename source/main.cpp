#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


#include "process.h"
#include "readcsv.h"


typedef std::vector<mol_info> mols;


void print_input(const Matrix<mol_info> &points){
    for(auto &row : points){
        for(auto &mol : row){
            std::cout << mol.first << ',' << mol.second << ' ';
        }
        std::cout << std::endl;
    }
}


void print_mols(std::ostream& out_stream, const std::list<Pattern> &pat_list){
    for(auto &pat : pat_list){
        if(!pat.molecules.empty()){
            out_stream << pat.molecules.front();
            for(auto it = pat.molecules.begin() + 1; it != pat.molecules.end(); ++it){
                out_stream << ',' << *it;
            }
            out_stream << std::endl;
        }
    }
}

void print_points(std::ostream& out_stream, const std::list<Pattern> &pat_list){
    for(auto &pat : pat_list){
        if(!pat.points.empty()){
            out_stream << pat.points.front();
            for(auto it = std::next(pat.points.begin()); it != pat.points.end(); ++it){
                out_stream << ',' << *it;
            }
            out_stream << std::endl;
        }
    }
}


void print_output(const std::list<Pattern> &out, std::string output_file, int k){

    if(output_file == ""){
        std::cout << "Molecules:" << std::endl;
        print_mols(std::cout, out);
        std::cout << "Points:" << std::endl;
        print_points(std::cout, out);
    }
    else{
        std::ofstream out_patterns, out_points;
        std::string out_patterns_file = output_file + "_mols_" + patch::to_string(k);
        out_patterns.open(out_patterns_file);

        if(out_patterns.is_open()){
            print_mols(out_patterns, out);
            out_patterns.close();
        }
        else{
            std::cout << "Error opening file " << out_patterns_file << std::endl;
        }

        std::string out_points_file = output_file + "_points_" + patch::to_string(k);
        out_points.open(out_points_file);
        if(out_points.is_open()){
            print_points(out_points, out);
            out_points.close();
        }
        else{
            std::cout << "Error opening file " << out_points_file << std::endl;
        }
    }

}


void reverse_matrix(Matrix<mol_info> &points){
    for(auto &row: points){
        std::reverse(row.begin(), row.end());
    }
}


void obtain_patterns(const Matrix<mol_info> &points, std::list<Pattern> &out, int k){
    clock_t t;
    Graph g;

    t = clock();
    build_graph(g, points, k);
    t = clock() - t;
    std::cout << "Build Graph: " << ((float)t)/CLOCKS_PER_SEC << "s\n";

    t = clock();
    level1(g, out);
    t = clock() - t;
    std::cout << "Dynamic programming: " << ((float)t)/CLOCKS_PER_SEC << "s\n";

    t = clock();
    post_process(out);
    t = clock() - t;
    std::cout << "Post process: " << ((float)t)/CLOCKS_PER_SEC << "s\n";
}


int main (int argc, char **argv){
    std::list<Pattern> out_max, out_min;
    unsigned int k = 1;
    char *cvalue = NULL;
    std::string input_file, output_file;
    int c;
    opterr = 0;

    bool min = false;
    input_file = "";
    output_file = "";

    while ((c = getopt(argc, argv, "mk:f:o:")) != -1){
        switch (c)
        {
            case 'm':
                min = true;
                break;
            case 'k':
                std::stringstream(optarg) >> k;
                break;
            case 'f':
                cvalue = optarg;
                input_file = std::string(cvalue);
                break;
            case 'o':
                cvalue = optarg;
                output_file = std::string(cvalue);
                break;
            case '?':
                if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    }

    Matrix<mol_info> points;

    build_matrix_from_csv(input_file, &points, min);

    if(k > (points[0].size() - 1)/2 ){
        std::cerr << "Value of k greater than half the number of molecules ("
             << points[0].size() << ")." << std::endl;
        return 0;
    }

    obtain_patterns(points, out_max, k);

    print_output(out_max, output_file, k);

    return 0;
}