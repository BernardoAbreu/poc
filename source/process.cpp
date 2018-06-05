#include "process.h"
#include "util.h"
#include <iostream>
using namespace std;


void insert_map(int point, Graph &graph,
                const vector<mol_info> &point_mols, 
                HashMolMap &mol_map,
                const vector<int> &mol_set,
                int level,
                int min_group_size,
                Node **last){

    Node *np;

    int mol_size = level + min_group_size;

    string key = join(mol_set, ',', mol_size);

    int current_mol = point_mols[mol_size - 1].first;
    double cur_attr = point_mols[mol_size - 1].second;
    double next_attr = point_mols[mol_size].second;
    double gap = cur_attr - next_attr;

    pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));
    if(search_result.second){
        Pattern pat(cur_attr, mol_set, mol_size);

        Node n(point_mols.size(), pat);

        np = graph.insert(level, n);

        (search_result.first)->second = np;
    }
    else{
        np = (search_result.first)->second;
    }

    np->pat.add_gap(gap);
    np->pat.add_point(point);

    if((*last) != NULL ){
        (*last)->add_child(current_mol, np);
    }

    (*last) = np;
}


void apply_sqrt(Graph &graph){

    for(auto &level : graph.level){
        for(auto &node : level){
            node.pat.quality = sqrt(node.pat.quality);
            node.pat.best_quality = node.pat.quality;
        }
    }
}


void add_vertices_edges_hashed(Graph &graph, const vector<vector<mol_info> > &points, int min_group_size){
    int level_size = graph.level.size();

    vector<int> mol_set(level_size + min_group_size - 1);

    int current_mol;

    Node *last;
    HashMolMap mol_map;

    int point = 0;
    for (auto &point_mols: points){
        last = NULL;
        for(int k = 0; k < min_group_size - 1; k++){
            current_mol = point_mols[k].first;
            insert_sorted(mol_set, current_mol, k + 1);
        }

        for (int level = 0; level < level_size ; level++){
            current_mol = point_mols[level + min_group_size - 1].first;
            insert_sorted(mol_set, current_mol, level + min_group_size);
            insert_map(point, graph, point_mols, mol_map, mol_set, level, min_group_size, &last);
        }
        point++;
    }

    apply_sqrt(graph);
}


void build_graph(Graph &graph, const vector<vector<mol_info> > &points, int min_group_size){
    if(min_group_size < 1) min_group_size = 1;

    int level_size = points[0].size() - 2 * min_group_size + 1;

    graph.level.resize(level_size);

    add_vertices_edges_hashed(graph, points, min_group_size);
}



void level1max(Graph &g, list<Pattern> &sel){
    bool possible;

    // First step - going down
    for(auto &level : g.level){
        for(auto &node : level){
            for(auto &child : node.next){
                child->pat.best_quality = max(node.pat.best_quality, child->pat.best_quality);
            }
        }
    }

    // std::cout << g << std::endl;

    // Second step - going up
    for (auto it = g.level.rbegin(); it != g.level.rend(); ++it) {
        for(auto &node : *it){
            possible = (node.pat.quality >= node.pat.best_quality);
            node.pat.best_quality = node.pat.quality;

            for(auto &child : node.next){
                node.pat.best_quality = max(node.pat.best_quality, child->pat.best_quality);
            }

            if(possible && (node.pat.quality == node.pat.best_quality)){
                sel.push_back(node.pat);
                node.pat.best_quality++;
            }
        }
    }
}


void level1min(Graph &g, list<Pattern> &sel){
    bool possible;

    // First step - going down
    for(auto &level : g.level){
        for(auto &node : level){
            for(auto &child : node.next){
                child->pat.best_quality = min(node.pat.best_quality, child->pat.best_quality);
            }
        }
    }

    // Second step - going up
    for (auto it = g.level.rbegin(); it != g.level.rend(); ++it) {
        for(auto &node : *it){
            possible = (node.pat.quality <= node.pat.best_quality);
            node.pat.best_quality = node.pat.quality;

            for(auto &child : node.next){
                node.pat.best_quality = min(node.pat.best_quality, child->pat.best_quality);
            }

            if(possible && (node.pat.quality == node.pat.best_quality)){
                sel.push_back(node.pat);
                node.pat.best_quality++;
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


void post_process(list<Pattern> &selected){
    post_process(selected, true);
}


bool is_sub_or_sup(const Pattern &pat1, const Pattern &pat2){
    auto& small = (pat1.molecules.size() > pat2.molecules.size()) ? pat2.molecules : pat1.molecules;
    auto& big = (pat1.molecules.size() > pat2.molecules.size()) ? pat1.molecules : pat2.molecules;

    return std::includes(big.begin(), big.end(), small.begin(), small.end());
}


void post_process2(list<Pattern> &selected, bool max){
    selected.sort((max) ? maxcmp : mincmp);

    for (auto it = selected.begin(); it != std::prev(selected.end()); ++it){
        for (auto jt = std::next(it); jt != selected.end();){
            if(is_sub_or_sup(*it, *jt)){
                jt = selected.erase(jt);
            }
            else{
                jt++;
            }
        }
    }

}


void post_process(list<Pattern> &selected, bool max){

    selected.sort((max) ? maxcmp : mincmp);

    vector<bool> not_valid(selected.size());

    int i = 0;
    for (auto it = selected.begin(); it != std::prev(selected.end()); ++it, i++){
        int j = i + 1;
        for (auto jt = std::next(it); jt != selected.end(); ++jt, j++){
            if(!not_valid[j]){
                not_valid[j] = is_sub_or_sup(*it, *jt);
            }
        }
    }

    i = 0;
    for (auto it = selected.begin(); it != selected.end(); i++){
        if(not_valid[i]){
            it = selected.erase(it);
        }
        else{
            it++;
        }
    }

}
