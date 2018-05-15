#include "process.h"
#include "util.h"

using namespace std;


void insert_map(int point, Graph &graph,
                const vector<mol_info> &point_mols, 
                HashMolMap &mol_map,
                const vector<int> &mol_set,
                string key,
                int current_mol, int level, int min_group_size, Node **last){

    Node *np;

    double cur_attr = point_mols[level].second;
    double next_attr = point_mols[level + 1].second;
    double gap = cur_attr - next_attr;

    pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

    if(search_result.second){
        Pattern pat(key, cur_attr, mol_set, level + min_group_size);
        Node n(point_mols.size(), pat);

        graph.level[level - min_group_size + 1].push_back(n);

        np = &(graph.level[level - min_group_size + 1].back());

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

    if(min_group_size < 1) min_group_size = 1;

    vector<int> mol_set(level_size + min_group_size);

    string key;
    int current_mol;

    Node *last;

    HashMolMap mol_map;

    pair<HashMolMap::iterator, bool> search_result;
    int point = 0;
    for (auto &point_mols : points){
        last = NULL;

        for(int k = 0; k < min_group_size; k++){
            current_mol = point_mols[k].first;
            insert_sorted(mol_set, current_mol, k + 1);
        }
        key = join(mol_set, ',', min_group_size);
        insert_map(point, graph, point_mols, mol_map, mol_set, key, current_mol, min_group_size - 1, min_group_size, &last);

        for (int level = min_group_size; level < level_size ; level++){
            current_mol = point_mols[level].first;
            insert_sorted(mol_set, current_mol, level + 1);
            key = join(mol_set, ',', level + 1);
            insert_map(point, graph, point_mols, mol_map, mol_set, key, current_mol, level, min_group_size, &last);
        }
        point++;
    }

    apply_sqrt(graph);
}


void build_graph(Graph &graph, const vector<vector<mol_info> > &points, int min_group_size){
    int level_size = points[0].size() - 2 * min_group_size + 1;

    graph.level.resize(level_size);

    add_vertices_edges_hashed(graph, points, min_group_size);
}


void level1max(Graph &g, list<Pattern> &sel){
    bool possible;

    // First step - going down
    for(auto &level : g.level){
        for(auto &node : level){
            node.pat.best_quality = node.pat.quality;
            for(auto &child : node.next){
                child->pat.best_quality = max(node.pat.best_quality, child->pat.best_quality);
            }
        }
    }

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


void level1min(Graph &g, list<Pattern> sel){
    bool possible;

    // First step - going down
    for(auto &level : g.level){
        for(auto &node : level){
            node.pat.best_quality = node.pat.quality;
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


void post_process(list<Pattern> &selected, bool max){
    selected.sort((max) ? maxcmp : mincmp);

    for (auto it = selected.begin(); it != std::prev(selected.end()); ++it){
        for (auto jt = it; jt != std::prev(selected.end());){
            auto next = std::next(jt);
            if(is_sub_or_sup(*it, *next)){
                selected.erase(next);
            }
            else{
                jt++;
            }
        }
    }

}
