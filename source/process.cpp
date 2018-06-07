#include "process.h"

using namespace std;


Node* insert_graph(Graph &graph, HashMolMap &mol_map,
                 const vector<int> &mol_set,
                 double cur_attr, int mol_size,
                 int level, int total_mols){

    Node *vertex;
    string key = join(mol_set, ',', mol_size);

    pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));
    if(search_result.second){
        Pattern pat(cur_attr, mol_set, mol_size);
        Node n(pat, total_mols);

        vertex = graph.insert(level, n);
        (search_result.first)->second = vertex;
    }
    else{
        vertex = (search_result.first)->second;
    }
    return vertex;
}

void update_vertex(Node *vertex, int point, mol_info &cur_mol,
                   mol_info &next_mol, Node *last){

    double gap = std::abs(cur_mol.second - next_mol.second);

    vertex->pat.add_gap(gap * gap);
    vertex->pat.add_point(point);

    if(last != NULL){
        last->add_child(cur_mol.first, vertex);
    }
}


void apply_sqrt(Graph &graph){

    for(auto &level : graph.level){
        for(auto &node : level){
            node.pat.quality = sqrt(node.pat.quality);
            node.pat.best_quality = node.pat.quality;
        }
    }
}


void add_vertices_edges_hashed(Graph &graph,
                               const vector<vector<mol_info> > &points,
                               int min_group_size){
    int level_size = graph.level.size();

    HashMolMap mol_map;

    vector<int> mol_set(level_size + min_group_size - 1);

    int point = 0;
    for (auto &point_mols: points){
        Node *last_vertex = NULL;

        for(int k = 0; k < min_group_size - 1; k++){
            mol_info cur_mol = point_mols[k];
            insert_sorted(mol_set, cur_mol.first, k + 1);
        }

        for (int level = 0; level < level_size ; level++){
            int mol_size = level + min_group_size;

            mol_info cur_mol = point_mols[mol_size - 1];
            mol_info next_mol = point_mols[mol_size];

            insert_sorted(mol_set, cur_mol.first, mol_size);
            
            Node *vertex = insert_graph(graph, mol_map, mol_set, cur_mol.second,
                                  mol_size, level, point_mols.size());
            update_vertex(vertex, point, cur_mol, next_mol, last_vertex);
            last_vertex = vertex;
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
