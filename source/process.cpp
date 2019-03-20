#include "process.h"
#include <iostream>



Node* insert_graph(Graph &graph, HashMolMap &mol_map,
                 int mol_set[],
                 double cur_attr, int mol_size,
                 int level, int total_mols){

    Node *vertex;

    std::string key = compress_array(mol_set, mol_size);

    std::pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

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


void update_vertex(Node *vertex, int point, index_value &cur_mol,
                   index_value &next_mol, Node *last){

    double gap = std::abs(cur_mol.second - next_mol.second);

    vertex->pat.add_gap(gap * gap);
    vertex->pat.add_point(point);

    if(last != NULL){
        last->add_child(cur_mol.first, vertex);
    }
}


void add_vertices_edges_hashed(Graph &graph,
                               index_value **matrix,
                               const std::pair<unsigned int, unsigned int> dimensions,
                               int min_group_size){
    int level_size = graph.level.size();

    HashMolMap mol_map;

    int mol_set[level_size + min_group_size - 1];

    int point = 0;
    for(unsigned int i = 0; i < dimensions.first; i++){
        Node *last_vertex = NULL;

        for(int k = 0; k < min_group_size - 1; k++){
            index_value cur_mol = matrix[i][k];
            insert_sorted(mol_set, cur_mol.first, k);
        }

        for (int level = 0; level < level_size ; level++){
            int mol_size = level + min_group_size;

            index_value cur_mol = matrix[i][mol_size - 1];
            index_value next_mol = matrix[i][mol_size];

            insert_sorted(mol_set, cur_mol.first, mol_size - 1);
            Node *vertex = insert_graph(graph, mol_map, mol_set, cur_mol.second,
                                        mol_size, level, dimensions.second);

            update_vertex(vertex, point, cur_mol, next_mol, last_vertex);
            last_vertex = vertex;

        }
        point++;
        delete[] matrix[i];
    }

    for(auto &level : graph.level){
        for(auto &node : level){
            node.pat.quality = sqrt(node.pat.quality);
            node.pat.best_quality = node.pat.quality;
        }
    }
}


void build_graph(Graph &graph, index_value **matrix,
                 const std::pair<unsigned int, unsigned int> &dimensions,
                 unsigned int min_group_size){

    if(min_group_size < 1) min_group_size = 1;

    int level_size = dimensions.second - 2 * min_group_size + 1;

    graph.level.resize(level_size);
    add_vertices_edges_hashed(graph, matrix, dimensions, min_group_size);
}


void build_graph_from_file(Graph &graph, std::string filename, unsigned int min_group_size){

    if(min_group_size < 1) min_group_size = 1;

    std::ifstream myfile(filename.c_str());

    if (myfile.is_open()){
        std::string line;

        getline(myfile, line);
        std::vector<int> aux = split<std::vector<int> >(line, ',');
        std::pair<unsigned int, unsigned int> dimensions = std::make_pair(aux[0], aux[1]);

        if(min_group_size > (dimensions.second - 1)/2 ){
            std::cerr << "Value of k greater than half the number of molecules ("
                 << dimensions.second << ")." << std::endl;
            exit(0);
        }

        int total_mols = aux[2];

        int level_size = dimensions.second - 2 * min_group_size + 1;

        graph.level.resize(level_size);

        HashMolMap mol_map;

        int mol_set_size = level_size + min_group_size - 1;
        int mol_set[mol_set_size];
        std::string row[dimensions.second];
        double pair_vector[2];
        index_value fixed_row[dimensions.second];

        int point = 0;
        for(unsigned int i = 0; i < dimensions.first; i++){

            getline(myfile, line);
            split<std::string >(line, ' ', row);
            for(unsigned int temp_index = 0; temp_index < dimensions.second; temp_index++){
                split<double>(row[temp_index], ',', pair_vector);
                fixed_row[temp_index] = std::make_pair((int)pair_vector[0], pair_vector[1]);
            }

            Node *last_vertex = NULL;

            for(unsigned int k = 0; k < min_group_size - 1; k++){
                index_value cur_mol = fixed_row[k];
                insert_sorted(mol_set, cur_mol.first, k);
            }

            for (int level = 0; level < level_size ; level++){

                int mol_size = level + min_group_size;

                index_value cur_mol = fixed_row[mol_size - 1];
                index_value next_mol = fixed_row[mol_size];

                insert_sorted(mol_set, cur_mol.first, mol_size - 1);

                Node *vertex = insert_graph(graph, mol_map, mol_set, cur_mol.second,
                                            mol_size, level, total_mols);

                update_vertex(vertex, point, cur_mol, next_mol, last_vertex);

                last_vertex = vertex;
            }

            point++;
        }

        for(auto &level : graph.level){
            for(auto &node : level){
                node.pat.quality = sqrt(node.pat.quality);
                node.pat.best_quality = node.pat.quality;
            }
        }

        myfile.close();
    }
    else{
        std::cerr << "Unable to open file"; 
    }
}

void level1(Graph &g, std::list<Pattern> &sel){
    bool possible;

    // First step - going down
    for(auto &level : g.level){
        for(auto &node : level){
            for(auto &child : node.next){
                child->pat.best_quality = std::max(node.pat.best_quality, child->pat.best_quality);
            }
        }
    }

    // Second step - going up
    for (auto it = g.level.rbegin(); it != g.level.rend(); ++it) {
        for(auto &node : *it){
            possible = (node.pat.quality >= node.pat.best_quality);
            node.pat.best_quality = node.pat.quality;

            for(auto &child : node.next){
                node.pat.best_quality = std::max(node.pat.best_quality, child->pat.best_quality);
            }

            if(possible && (node.pat.quality == node.pat.best_quality)){
                sel.push_back(node.pat);
                node.pat.best_quality++;
            }
        }
    }
}


bool maxcmp (Pattern i, Pattern j) { return (i.quality > j.quality); }


bool is_sub_or_sup(const Pattern &pat1, const Pattern &pat2){
    int pat1_size = pat1.get_mol_size();
    int pat2_size = pat2.get_mol_size();

    auto& small = (pat1_size > pat2_size) ? pat2.molecules : pat1.molecules;
    auto& big = (pat1_size > pat2_size) ? pat1.molecules : pat2.molecules;

    int small_size, big_size;
    if(pat1_size > pat2_size){
        big_size = pat1_size;
        small_size = pat2_size;
    }
    else{
        big_size = pat2_size;
        small_size = pat1_size;
    }

    return std::includes(big, big + big_size, small, small + small_size);
}


void post_process(std::list<Pattern> &selected){

    selected.sort(maxcmp);

    std::vector<bool> not_valid(selected.size());

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
