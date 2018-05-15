#include "process.h"
#include "util.h"

using namespace std;


void insert_map(int point, Graph *g, const vector<vector<mol_info> > &points, 
    HashMolMap *mol_map, vector<int> *mol_set, string key, 
    int current_mol, int level, int min_group_size, Node **last){

    double cur_attr, next_attr, gap;

    Node *np;
    Node n;
    Pattern pat;

    pair<HashMolMap::iterator, bool> search_result;

    cur_attr = points[point][level].second;
    next_attr = points[point][level+1].second;
    gap = cur_attr - next_attr;

    search_result = mol_map->insert(HashMolMap::value_type(key, NULL));

    if(search_result.second){
        pat.mol_set = key;
        pat.quality = 0;
        pat.best_quality = 0;
        pat.gap = 0;
        pat.limit = cur_attr;
        pat.molecules = vector<int>(mol_set->begin(), mol_set->begin()+level+min_group_size);
        n.pat = pat;

        g->level[level-min_group_size + 1].push_back(n);

        np = &(g->level[level - min_group_size + 1].back());

        for(int k = 0; k < 4; k++){
            np->children.push_back(0);
        }

        (search_result.first)->second = np;
    }
    else{
        np = (search_result.first)->second;
    }

    np->pat.quality += (gap*gap);

    np->pat.gap += gap;

    // np->pat.limit =

    np->pat.points.push_back(point);


    if((*last) != NULL ){
        if(!(((*last)->children[current_mol>>5]>>(current_mol%32))&1)){
            (*last)->children[current_mol>>5] |= 1<<(current_mol & 31);
            (*last)->next.push_back(np);
        }
    }
    (*last) = np;

}

void apply_sqrt(Graph *g){
    vector<list<Node> >::iterator it;
    list<Node>::iterator jt;

    for (it=g->level.begin(); it != g->level.end(); ++it){
        for (jt=(*it).begin(); jt != (*it).end(); ++jt){
            (*jt).pat.quality = sqrt((*jt).pat.quality);
            (*jt).pat.best_quality = (*jt).pat.quality;
        }
    }
}

void add_vertices_edges_hashed(Graph *g, const vector<vector<mol_info> > &points, int min_group_size){
    int level_size = g->level.size();

    if(min_group_size < 1) min_group_size = 1;

    vector<int> mol_set(level_size+min_group_size);

    string key;
    int current_mol, points_size;

    Node *last;
    Node n;

    HashMolMap mol_map;

    points_size = points.size();
    pair<HashMolMap::iterator, bool> search_result;
    for (int i = 0; i < points_size; i++){
        last = NULL;

        for(int k = 0; k < min_group_size; k++){
            current_mol = points[i][k].first;
            insert_sorted(mol_set, current_mol, k+1);
        }
        key = join(mol_set, ',', min_group_size);
        insert_map(i, g, points, &mol_map, &mol_set, key, current_mol, min_group_size-1, min_group_size, &last);

        for (int level = min_group_size; level < level_size ; level++){
            current_mol = points[i][level].first;
            insert_sorted(mol_set, current_mol, level+1);
            key = join(mol_set, ',', level+1);
            insert_map(i, g, points, &mol_map, &mol_set, key, current_mol, level, min_group_size, &last);
        }
    }

    apply_sqrt(g);
}


void build_graph(Graph *g, const vector<vector<mol_info> > &points, int min_group_size){
    int level_size = points[0].size() - 2 * min_group_size + 1;

    g->level.resize(level_size);

    add_vertices_edges_hashed(g, points, min_group_size);
}


void level1max(Graph &g, list<Pattern> &sel){
    list<Node>::iterator it;
    list<Node*>::iterator jt;

    bool possible;
    // int level_size = g.level.size();

    // Node *n;

    // First step - going down
    for(auto &level : g.level){
    // for(int i = 0; i < level_size; i++){
        for(auto &node : level){
        // for (it=g->level[i].begin(); it != g->level[i].end(); ++it){
            // n = &(*it);

            // n->pat.best_quality = n->pat.quality;
            node.pat.best_quality = node.pat.quality;

            // for(jt=n->next.begin(); jt != n->next.end(); ++jt){
            //     (**jt).pat.best_quality = max(n->pat.best_quality, (**jt).pat.best_quality);
            // }
            for(auto &child : node.next){
                child->pat.best_quality = max(node.pat.best_quality, child->pat.best_quality);
            }
        }
    }

    // Second step - going up
    // for (int i = level_size-1; i >= 0; i--){
    for (auto it = g.level.rbegin(); it != g.level.rend(); ++it) {
        for(auto &node : *it){
        // for (it=g->level[i].begin(); it != g->level[i].end(); ++it){
            // n = &(*it);
            // possible = (n->pat.quality >= n->pat.best_quality);
            // n->pat.best_quality = n->pat.quality;
            possible = (node.pat.quality >= node.pat.best_quality);
            node.pat.best_quality = node.pat.quality;

            // for(jt=n->next.begin(); jt != n->next.end(); ++jt){
            //     n->pat.best_quality = max(n->pat.best_quality, (**jt).pat.best_quality);
            // }
            for(auto &child : node.next){
                node.pat.best_quality = max(node.pat.best_quality, child->pat.best_quality);
            }

            // if(possible && (n->pat.quality == n->pat.best_quality)){
            //     sel.push_back(n->pat);
            //     n->pat.best_quality++;
            // }
            if(possible && (node.pat.quality == node.pat.best_quality)){
                sel.push_back(node.pat);
                node.pat.best_quality++;
            }
        }
    }

}



void level1min(Graph &g, list<Pattern> sel){
    list<Node>::iterator it;
    list<Node*>::iterator jt;

    bool possible;
    int level_size = g.level.size();

    Node *n;

    // First step - going down
    for(int i = 0; i < level_size; i++){
        for (it=g.level[i].begin(); it != g.level[i].end(); ++it){
            n = &(*it);

            n->pat.best_quality = n->pat.quality;

            for(jt=n->next.begin(); jt != n->next.end(); ++jt){
                (**jt).pat.best_quality = min(n->pat.best_quality, (**jt).pat.best_quality);
            }
        }
    }

    // Second step - going up
    for (int i = level_size-1; i >= 0; i--){
        for (it=g.level[i].begin(); it != g.level[i].end(); ++it){
            n = &(*it);
            possible = (n->pat.quality <= n->pat.best_quality);
            n->pat.best_quality = n->pat.quality;

            for(jt=n->next.begin(); jt != n->next.end(); ++jt){
                n->pat.best_quality = min(n->pat.best_quality, (**jt).pat.best_quality);
            }

            if(possible && (n->pat.quality == n->pat.best_quality)){
                sel.push_back(n->pat);
                n->pat.best_quality++;
            }
        }
    }

}


void level1(Graph &g, list<Pattern> &sel){
    level1max(g, sel);
}


void level1(Graph &g, list<Pattern> &sel, bool max){
    if(max){
        level1max(g, sel);
    }
    else{
        level1min(g, sel);
    }
}



bool maxcmp (Pattern i, Pattern j) { return (i.quality > j.quality); }
bool mincmp (Pattern i, Pattern j) { return (i.quality < j.quality); }


bool is_subset(const std::vector<int> &small, const std::vector<int> &big){
    if(small.size() > big.size()) return false;

    return std::includes(big.begin(), big.end(), small.begin(), small.end());
}


void post_process(list<Pattern> &selected){
    post_process(selected, true);
}


bool is_dup(const Pattern &pat1, const Pattern &pat2){
    return is_subset(pat1.molecules, pat2.molecules) || is_subset(pat2.molecules, pat1.molecules);
}


// void remove_dup(std::forward_list<int> &mylist){
//     int size = 0;
//     for(auto &b : mylist){
//         size++;
//     }

//     int i = 0;
//     for ( auto it = mylist.begin(); i < size - 1; ++it, ++i){
//         int j = i;
//         for ( auto jt = it; j < size - 1; ++jt, ++j ){
//             if(is_dup(*it, *std::next(jt))){
//                 mylist.erase_after(jt);
//                 size--;
//             }
//             print(mylist);
//         }
//     }

// }

#include <iostream>
void remove_dup(std::list<Pattern> &selected){
    for (auto it = selected.begin(); it != std::prev(selected.end()); ++it){
        for (auto jt = it; jt != std::prev(selected.end());){
            auto next = std::next(jt);
            if(is_dup(*it, *next)){
                selected.erase(next);
            }
            else{
                jt++;
            }
        }
    }

}

void post_process(list<Pattern> &selected, bool max){

    // sort(selected.begin(), selected.end(), (max) ? maxcmp : mincmp);
    selected.sort((max) ? maxcmp : mincmp);

    remove_dup(selected);

}
