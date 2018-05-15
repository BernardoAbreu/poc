#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>

#include "graph.h"
#include "process.h"
#include "readcsv.h"


typedef std::vector<mol_info> mols;
typedef std::vector<mols > matrix;

void print_input(const matrix &points){
    for(auto &row : points){
        for(auto &mol : row){
            std::cout << mol.first << ',' << mol.second << ' ';
        }
        std::cout << std::endl;
    }
}

void print_output(std::list<Pattern> &out, std::string output_file, int k){

    std::ofstream out_patterns, out_points, out_limit;

    std::string out_patterns_file = output_file + "_mols_" + patch::to_string(k);
    out_patterns.open(out_patterns_file);

    if(out_patterns.is_open()){
        for(auto &pat : out){
            out_patterns << pat.get_string() << std::endl;
        }
        out_patterns.close();
    }
    else{
        std::cout << "Error opening file " << out_patterns_file << std::endl;
    }

    std::string out_points_file = output_file + "_points_" + patch::to_string(k);
    out_points.open(out_points_file);
    if(out_points.is_open()){
        for(auto &pat : out){
            out_points << join(pat.points, ',') << std::endl;
        }
        out_points.close();
    }
    else{
        std::cout << "Error opening file " << out_points_file << std::endl;
    }

    std::string out_limit_file = output_file + "_limit_" + patch::to_string(k);
    out_limit.open(out_limit_file);
    if (out_limit.is_open()){
        for(auto &pat : out){
            out_limit << pat.limit << std::endl;
        }
        out_points.close();
    }
    else{
        std::cout << "Error opening file " << out_limit_file << std::endl;
    }
}



void reverse_matrix(matrix &points){
    for(auto &row: points){
        std::reverse(row.begin(), row.end());
    }
}


void obtain_patterns(const matrix &points, std::list<Pattern> &out, int k){
    Graph g;

    build_graph(g, points, k);

    level1(g, out);

    post_process(out);
}


int main (int argc, char **argv){
    std::list<Pattern> out_max, out_min;
    unsigned int k = 1;
    char *cvalue = NULL;
    std::string input_file, output_file;
    int c;
    opterr = 0;

    input_file = "C_X_TP_t3_T.txt";
    output_file = "out";

    while ((c = getopt(argc, argv, "k:f:o:")) != -1){
        switch (c)
        {
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

    matrix points;

    build_matrix_from_csv(input_file, &points);

    // print_input(&points);

    // reverse_matrix(&points);
    // print_input(&points);

    if(k > (points[0].size() - 1)/2 ){
        std::cout << "Value of k greater than half the number of molecules ("
             << points[0].size() << ")." << std::endl;
        return 0;
    }

    obtain_patterns(points, out_max, k);

    print_output(out_max, output_file, k);
    // print_output(out_max, output_file+"_max", k);

    // reverse_matrix(&points);
    // // print_input(&points);

    // obtain_patterns(&points, &out_min, k);

    // print_output(out_min, output_file+"_min", k);

    return 0;
}