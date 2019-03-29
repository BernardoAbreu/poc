#include <iostream>

#include <list>
#include <unistd.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

#include "process.h"
#include "readcsv.h"
#include "pattern.h"



void print_input(index_value **matrix, const std::pair<unsigned int, unsigned int> &dimensions){
    for(unsigned int i = 0; i < dimensions.first; i++){
        for (unsigned int j = 0; j < dimensions.second; j++){
            std::cout << matrix[i][j].first << ',' << matrix[i][j].second << ' ';
        }
        std::cout << std::endl;
    }
}


void print_rows(std::ostream& out_stream, const std::list<Pattern> &pat_list){
    for(auto &pat : pat_list){
        if(!pat.rows.empty()){
            out_stream << pat.rows.front();
            for(auto it = std::next(pat.rows.begin()); it != pat.rows.end(); ++it){
                out_stream << ',' << *it;
            }
            out_stream << std::endl;
        }
    }
}

void print_cols(std::ostream& out_stream, const std::list<Pattern> &pat_list){
    for(auto &pat : pat_list){
        if(!pat.cols.empty()){
            out_stream << pat.cols.front();
            for(auto it = std::next(pat.cols.begin()); it != pat.cols.end(); ++it){
                out_stream << ',' << *it;
            }
            out_stream << std::endl;
        }
    }
}


void print_output(const std::list<Pattern> &out, std::string &output_file){

    if(output_file == ""){
        std::cout << "Rows:" << std::endl;
        print_rows(std::cout, out);
        std::cout << "Cols:" << std::endl;
        print_cols(std::cout, out);
    }
    else{
        std::ofstream out_rows, out_cols;

        std::string out_rows_file = output_file + "_rows";
        out_rows.open(out_rows_file);
        if(out_rows.is_open()){
            print_rows(out_rows, out);
            out_rows.close();
        }
        else{
            std::cerr << "Error opening file " << out_rows_file << std::endl;
        }

        std::string out_cols_file = output_file + "_cols";
        out_cols.open(out_cols_file);
        if(out_cols.is_open()){
            print_cols(out_cols, out);
            out_cols.close();
        }
        else{
            std::cerr << "Error opening file " << out_cols_file << std::endl;
        }
    }

}


int main (int argc, char **argv){
    unsigned int k = 1;
    char *cvalue = NULL;
    std::string input_file, output_file;
    int c;
    opterr = 0;

    input_file = "";
    output_file = "";

    while ((c = getopt(argc, argv, "k:f:o:")) != -1){
        switch (c){
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
    std::list<Pattern> out;

    std::cout << "Begin:\n";

    t = clock();
    extract_patterns(input_file, k, out);
    t = clock() - t;
    std::cout << "Patterns extracted: " << ((float)t)/CLOCKS_PER_SEC << "s\n" << std::flush;;

    t = clock();
    post_process(out);
    t = clock() - t;
    std::cout << "Post process: " << ((float)t)/CLOCKS_PER_SEC << "s\n" << std::flush;

    if(output_file != ""){
        output_file += "_" + patch::to_string(k);
    }

    print_output(out, output_file);

    return 0;
}