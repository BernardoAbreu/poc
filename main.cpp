#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>

#include "graph.h"
#include "process.h"
#include "readcsv.h"


using namespace std;

typedef vector<mol_info> mols;
typedef vector<mols > matrix;

void print_input(matrix *points){

    for(matrix::iterator it = points->begin(); it != points->end(); it++){
        for (mols::iterator jt = (*it).begin(); jt != (*it).end(); ++jt){
            cout << (*jt).first << ',' << (*jt).second << ' ';
        }
        cout << endl;
    }
}

void print_output(list<pattern> &out, string output_file, int k){

    ofstream out_patterns, out_points;

    string out_patterns_file = output_file + "_patterns_" + patch::to_string(k);
    out_patterns.open (out_patterns_file);

    if (out_patterns.is_open()){
        for(list<pattern>::iterator it=out.begin(); it != out.end(); ++it){
            out_patterns << (*it).mol_set << endl;
        }
        out_patterns.close();
    }
    else{
        cout << "Error opening file " << out_patterns_file << endl;
    }

    string out_points_file = output_file + "_points_" + patch::to_string(k);
    out_points.open (out_points_file);
    if (out_points.is_open()){
        for(list<pattern>::iterator it=out.begin(); it != out.end(); ++it){
            out_points << join((*it).points, ',') << endl;
        }
        out_points.close();
    }
    else{
        cout << "Error opening file " << out_points_file << endl;
    }
}



void reverse_matrix(matrix *points){
    for(matrix::iterator it = points->begin(); it != points->end(); it++){
        reverse((*it).begin(), (*it).end());
    }
}


void obtain_patterns(matrix *points, list<pattern> *out, int k){
    Graph g;
    vector<pattern> selected;

    build_graph(&g, points, k);

    level1(&g, &selected, true);

    post_process(&selected, out, true);
}


int main (int argc, char **argv){
    list<pattern> out;
    unsigned int k = 1;
    char *cvalue = NULL;
    string input_file, output_file;
    int c;
    opterr = 0;

    input_file = "C_X_TP_t3_T.txt";
    output_file = "out.txt";

    while ((c = getopt (argc, argv, "k:f:o:")) != -1){
        switch (c)
        {
            case 'k':
                std::istringstream(optarg) >> k;
                break;
            case 'f':
                cvalue = optarg;
                input_file = string(cvalue);
                break;
            case 'o':
                cvalue = optarg;
                output_file = string(cvalue);
                break;
            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }
    }

    matrix points;

    build_matrix_from_csv(input_file, &points);

    print_input(&points);

    // reverse_matrix(&points);
    // print_input(&points);

    if(k > (points[0].size() - 1)/2 ){
        cout << "Value of k greater than half the number of molecules (" << points[0].size() << ")." << endl;
        return 0;
    }

    obtain_patterns(&points, &out, k);

    print_output(out, output_file, k);

    reverse_matrix(&points);
    print_input(&points);

    obtain_patterns(&points, &out, k);

    print_output(out, output_file, k);

    return 0;
}