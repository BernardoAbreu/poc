#include <iostream>

#include <vector>
#include <list>
#include <unistd.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

#include "process.h"
#include "readcsv.h"


typedef std::vector<index_value> mols;

template<typename T>
void print_container(T &v){
    for(auto &e: v){
        std::cout << e << ' ';
    }
    std::cout << std::endl;
}



void print_input(index_value **matrix, const std::pair<unsigned int, unsigned int> &dimensions){
    for(unsigned int i = 0; i < dimensions.first; i++){
        for (unsigned int j = 0; j < dimensions.second; j++){
            std::cout << matrix[i][j].first << ',' << matrix[i][j].second << ' ';
        }
        std::cout << std::endl;
    }
}


void print_rows(std::ostream& out_stream, const std::list<FinalPattern> &pat_list){
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

void print_cols(std::ostream& out_stream, const std::list<FinalPattern> &pat_list){
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


void print_output(const std::list<FinalPattern> &out, std::string output_file){

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
            std::cout << "Error opening file " << out_rows_file << std::endl;
        }

        std::string out_cols_file = output_file + "_cols";
        out_cols.open(out_cols_file);
        if(out_cols.is_open()){
            print_cols(out_cols, out);
            out_cols.close();
        }
        else{
            std::cout << "Error opening file " << out_cols_file << std::endl;
        }
    }

}

struct extraction_info{
    int col;
    int total_rows;
    int pat_index;
};

bool operator<( const extraction_info& a, const extraction_info&b ){
    return a.col < b.col;
}

bool operator<( const extraction_info& a, int b){
    return a.col < b;
}

void extract_pattern(const std::string &filename, int k,
                     std::list<std::pair<int, Pattern>> &out_aux,
                     std::list<FinalPattern> &out){

    int row, max_rows;
    std::list<extraction_info> not_extracted;
    std::vector<FinalPattern> final_pat(out_aux.size());

    int i = 0;
    for(auto &e: out_aux){
        final_pat[i] = FinalPattern(e.second);

        int col = final_pat[i].cols.back();
        not_extracted.insert(
            std::lower_bound(not_extracted.begin(), not_extracted.end(), col),
            {col, e.first + k, i});
        i++;
    }

    std::ifstream myfile(filename.c_str());

    if (myfile.is_open()){
        std::string line, tuple, value;
        getline(myfile, line);

        for(int i = 0; !not_extracted.empty(); i++){
            auto it = not_extracted.begin();

            getline(myfile, line);
            std::stringstream linestream(line);
            for(int col = 0; it != not_extracted.end(); col++){
                getline(linestream, tuple, ' ');
                if(col == (*it).col){
                    getline(std::stringstream(tuple), value, ',');
                    std::istringstream(value) >> row;
                    insert_sorted(final_pat[(*it).pat_index].rows, row);

                    if((*it).total_rows == (i + 1)){
                        it = not_extracted.erase(it);
                    }
                    else{
                        it++;
                    }
                }
            }

            // if(not_extracted.empty()){
            //     break;
            // }
        }

        // print_container<std::vector<FinalPattern>>(final_pat);
        std::copy(final_pat.begin(), final_pat.end(), std::back_inserter(out));
        // print_container<std::list<FinalPattern>>(out);
    }
    else{
        std::cerr << "Unable to open file"; 
    }
}


int main (int argc, char **argv){
    unsigned int k = 1;
    char *cvalue = NULL;
    std::string input_file, output_file;
    int c;
    opterr = 0;

    bool min = false;
    input_file = "";
    output_file = "";

    while ((c = getopt(argc, argv, "mk:f:o:")) != -1){
        switch (c){
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

    clock_t t;
    Graph g;
    std::list<std::pair<int, Pattern>> out_aux;
    std::list<FinalPattern> out;

    std::cout << "Begin:\n";

    t = clock();
    build_graph(g, input_file, k);
    t = clock() - t;
    std::cout << "Build Graph: " << ((float)t)/CLOCKS_PER_SEC << "s\n";
    // std::cout << g << std::endl;
    // std::cout << g.level.size();
    t = clock();
    level1(g, out_aux);
    t = clock() - t;
    std::cout << "Dynamic programming: " << ((float)t)/CLOCKS_PER_SEC << "s\n";

    // std::cout << "Hello: " << out_aux.size() << "\n";
    // for(auto &e: out_aux){
    //     std::cout << e.second << std::endl;
    // }

    t = clock();
    extract_pattern(input_file, k, out_aux, out);
    t = clock() - t;
    // std::cout << "Pattern extraction: " << ((float)t)/CLOCKS_PER_SEC << "s\n";
    //     for(auto &e: out){
    //     std::cout << e << std::endl;
    // }

    t = clock();
    post_process(out);
    t = clock() - t;
    std::cout << "Post process: " << ((float)t)/CLOCKS_PER_SEC << "s\n";

    print_output(out, output_file + "_" + patch::to_string(k));

    return 0;
}