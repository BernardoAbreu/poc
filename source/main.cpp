#include <iostream>

#include <vector>
#include <list>
#include <unistd.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


#include "process.h"
#include "readcsv.h"


typedef std::vector<index_value> mols;


void print_input(index_value **matrix, const std::pair<unsigned int, unsigned int> &dimensions){
    for(unsigned int i = 0; i < dimensions.first; i++){
        for (unsigned int j = 0; j < dimensions.second; j++){
            std::cout << matrix[i][j].first << ',' << matrix[i][j].second << ' ';
        }
        std::cout << std::endl;
    }
}


void print_mols(std::ostream& out_stream, const std::list<Pattern> &pat_list){
    for(auto &pat : pat_list){
        int mol_size = pat.get_mol_size();
        if(mol_size){
            out_stream << pat.molecules[0];
            for(int i = 1; i < mol_size; i++){
                out_stream << ',' << pat.molecules[i];
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
        std::cout << "Rows:" << std::endl;
        print_mols(std::cout, out);
        std::cout << "Cols:" << std::endl;
        print_points(std::cout, out);
    }
    else{
        std::ofstream out_patterns, out_points;
        std::string out_patterns_file = output_file + "_rows_" + patch::to_string(k);
        out_patterns.open(out_patterns_file);

        if(out_patterns.is_open()){
            print_mols(out_patterns, out);
            out_patterns.close();
        }
        else{
            std::cout << "Error opening file " << out_patterns_file << std::endl;
        }

        std::string out_points_file = output_file + "_cols_" + patch::to_string(k);
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
    bool merged_read = false;

    while ((c = getopt(argc, argv, "mrk:f:o:")) != -1){
        switch (c)
        {
            case 'm':
                min = true;
                break;
            case 'r':
                cvalue = optarg;
                merged_read=true;
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

    clock_t t;
    Graph g;
    std::cout << "Begin:\n";

    if(merged_read){
        t = clock();
        build_graph_from_file(g, input_file, k);
        t = clock() - t;
        std::cout << "Build Graph: " << ((float)t)/CLOCKS_PER_SEC << "s\n";
    }
    else{
        index_value **matrix;
        std::pair<unsigned int, unsigned int> dimensions = build_matrix_from_csv(input_file, matrix, min);

        if(k > (dimensions.second - 1)/2 ){
            std::cerr << "Value of k greater than half the number of molecules ("
                 << dimensions.second << ")." << std::endl;
            return 0;
        }

        t = clock();
        build_graph(g, matrix, dimensions, k);
        t = clock() - t;
        std::cout << "Build Graph: " << ((float)t)/CLOCKS_PER_SEC << "s\n";
        delete[] matrix;
    }




    t = clock();
    level1(g, out_max);
    t = clock() - t;
    std::cout << "Dynamic programming: " << ((float)t)/CLOCKS_PER_SEC << "s\n";


    t = clock();
    post_process(out_max);
    t = clock() - t;
    std::cout << "Post process: " << ((float)t)/CLOCKS_PER_SEC << "s\n";

    print_output(out_max, output_file, k);

    return 0;
}