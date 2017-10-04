#include <string>
#include <vector>
#include <list>

#include "graph.h"
#include "process.h"

using namespace std;

int main(){

    graph g;
    int max_points = 5;
    int max_mol = 4;

    g.level.resize(max_mol-1);
    vector<vector<mol_info> > points(max_points);

    points[0].push_back(make_pair(4,9));
    points[0].push_back(make_pair(3,8));
    points[0].push_back(make_pair(2,5));
    points[0].push_back(make_pair(1,2));


    points[1].push_back(make_pair(1,8));
    points[1].push_back(make_pair(2,6));
    points[1].push_back(make_pair(3,3));
    points[1].push_back(make_pair(4,2));

    points[2].push_back(make_pair(3,10));
    points[2].push_back(make_pair(2,5));
    points[2].push_back(make_pair(1,4));
    points[2].push_back(make_pair(4,3));
    
    points[3].push_back(make_pair(1,7));
    points[3].push_back(make_pair(2,6));
    points[3].push_back(make_pair(3,5));
    points[3].push_back(make_pair(4,1));
    
    points[4].push_back(make_pair(2,8));
    points[4].push_back(make_pair(4,3));
    points[4].push_back(make_pair(1,2));
    points[4].push_back(make_pair(3,1));


    // print_input(points);

    build_graph(&g, &points, max_mol-1);

    level_traverse(&g);

    vector<node> selected;
    list<node> out;

    level1(&g, &selected);

    cout << endl;
    level_traverse(&g);

    post_process(&selected, &out);

    cout << endl;
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