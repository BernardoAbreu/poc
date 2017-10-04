#include <algorithm>
#include "process.h"

using namespace std;

void level1(graph *g, vector<node> *sel){
    list<node>::iterator it;
    list<node*>::iterator jt;

    bool possible;
    int level_size = g->level.size();

    node *n;

    for(int i = 0; i < level_size; i++){
        for (it=g->level[i].begin(); it != g->level[i].end(); ++it){
            n = &(*it);

            for(jt=n->next.begin(); jt != n->next.end(); ++jt){
                (**jt).best_quality = max(n->best_quality, (**jt).best_quality);
            }
        }
    }


    for (int i = level_size-1; i >= 0; i--){
        for (it=g->level[i].begin(); it != g->level[i].end(); ++it){
            n = &(*it);
            possible = (n->quality >= n->best_quality);
            n->best_quality = n->quality;

            for(jt=n->next.begin(); jt != n->next.end(); ++jt){
                n->best_quality = max(n->best_quality, (**jt).best_quality);
            }

            if(possible && (n->quality == n->best_quality)){
                sel->push_back(*n);
                n->best_quality++;
            }
        }
    }
}


bool myfunction (node i,node j) { return (i.quality > j.quality); }


bool is_subset(vector<int> *small, vector<int> *big){
    if(small->size() > big->size()) return false;

    vector<int>::iterator small_it;
    vector<int>::iterator big_it;


    for (big_it=big->begin(), small_it=small->begin();
         big_it != big->end() && small_it != small->end(); ++big_it){
        if((*big_it) == (*small_it)) small_it++;
    }

    return small_it == small->end();
}

void post_process(vector<node> *selected, list<node> *out){
    sort(selected->begin(), selected->end(), myfunction);
    
    int size = selected->size();
    int* valid = new int[size];

    for(int i = 0; i < size; i++){
        valid[i] = 1;
    }

    // cout << endl;
    // for(vector<node>::iterator it=selected->begin(); it != selected->end(); ++it){
    //     cout << (*it).mol_set << " - ";
    // }
    // cout << endl;

    for(int i = 0; i < size; i++){
        for(int j = i+1; j < size; j++){
            // cout << (*selected)[i].mol_set;

            // if(is_subset(&((*selected)[i].molecules), &((*selected)[j].molecules))){
            //     cout << " is subset of " << (*selected)[j].mol_set << '.';
            //     valid[j] = 0;
            // }
            // else if(is_subset(&((*selected)[j].molecules), &((*selected)[i].molecules))){
            //     cout << " is superset of " << (*selected)[j].mol_set << '.';
            //     valid[j] = 0;
            // }
            // 
            if((is_subset(&((*selected)[i].molecules), &((*selected)[j].molecules))) ||
               (is_subset(&((*selected)[j].molecules), &((*selected)[i].molecules)))){
                valid[j] = 0;
            }

            // cout << endl;
        }
    }


    for(int i = 0; i < size; i++){
        if(valid[i]){
            out->push_back((*selected)[i]);
        }
    }

}
