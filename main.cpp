#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include <sstream>

#include "graph.h"
#include "process.h"
#include "readcsv.h"


using namespace std;


void print_input(vector<vector<mol_info> > *points){

    for(vector<vector<mol_info> >::iterator it = points->begin(); it != points->end(); it++){
        for (vector<mol_info>::iterator jt = (*it).begin(); jt != (*it).end(); ++jt){
            cout << (*jt).first << ',' << (*jt).second << ' ';
        }
        cout << endl;
    }
}


int main (int argc, char **argv){

    graph g;
    int k = 1;
    char *cvalue = NULL;
    string input_file = "C_X_TP_t3_T.txt";
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "k:f:")) != -1){
        switch (c)
        {
            case 'k':
                std::istringstream(optarg) >> k;
                // k = optarg;
                break;
            case 'f':
                cvalue = optarg;
                input_file = string(cvalue);
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

    
    vector<vector<mol_info> > points;

    // string input_file = "C_X_TP_t3_T.txt";
    // string input_file = "test.txt";
    build_matrix_from_csv(input_file, &points);

    // print_input(&points);

    build_graph(&g, &points, k);

    // level_traverse(&g);

    vector<node> selected;
    list<node> out;

    level1(&g, &selected);

    // cout << endl;
    // level_traverse(&g);

    post_process(&selected, &out);

    // cout << endl;
    for(vector<node>::iterator it=selected.begin(); it != selected.end(); ++it){
        cout << (*it).mol_set << " - ";
    }
    cout << endl;

    cout << endl;
    for(list<node>::iterator it=out.begin(); it != out.end(); ++it){
        cout << (*it).mol_set << " - ";
    }
    cout << endl;
    return 0;
}