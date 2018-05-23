#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


#include "graph.h"
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

    // std::string out_limit_file = output_file + "_limit_" + patch::to_string(k);
    // out_limit.open(out_limit_file);
    // if (out_limit.is_open()){
    //     for(auto &pat : out){
    //         out_limit << pat.limit << std::endl;
    //     }
    //     out_points.close();
    // }
    // else{
    //     std::cout << "Error opening file " << out_limit_file << std::endl;
    // }
}


void reverse_matrix(Matrix<mol_info> &points){
    for(auto &row: points){
        std::reverse(row.begin(), row.end());
    }
}


void obtain_patterns(const Matrix<mol_info> &points, std::list<Pattern> &out, int k, bool max){
    clock_t t;
    Graph g;

    t = clock();
    build_graph(g, points, k);
    t = clock() - t;
    std::cout << "Build Graph: " << ((float)t)/CLOCKS_PER_SEC << "s\n";

    t = clock();
    level1(g, out, max);
    t = clock() - t;
    std::cout << "Dynamic programming: " << ((float)t)/CLOCKS_PER_SEC << "s\n";

    t = clock();
    post_process(out, max);
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

    bool max = true;
    input_file = "";
    output_file = "out";

    while ((c = getopt(argc, argv, "mk:f:o:")) != -1){
        switch (c)
        {
            case 'm':
                max = false;
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

    build_matrix_from_csv(input_file, &points, max);

    if(k > (points[0].size() - 1)/2 ){
        std::cerr << "Value of k greater than half the number of molecules ("
             << points[0].size() << ")." << std::endl;
        return 0;
    }

    obtain_patterns(points, out_max, k, max);

    print_output(out_max, output_file, k);

    return 0;
}