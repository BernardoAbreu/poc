#include "process.h"
#include <iostream>


// template<typename T>
// void print_array(T *arr, int size){
//     for(int i = 0; i < size; i++){
//         std::cout << arr[i] << ' ';
//     }
//     std::cout << std::endl;
// }

Node* insert_graph(Graph &graph, HashMolMap &mol_map, int mol_set[], int mol_size, int level, int total_mols){

    Node *vertex;

    std::string key = compress_array(mol_set, mol_size);
    // std::cout << key << std::endl;
    std::pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

    if(search_result.second){
        vertex = graph.insert(level, total_mols);
        (search_result.first)->second = vertex;
    }
    else{
        vertex = (search_result.first)->second;
    }
    return vertex;
}


void update_vertex(Node *vertex, int point, index_value &cur_mol, double next_value, Node *last){

    double gap = std::abs(cur_mol.second - next_value);

    vertex->pat.add_gap(gap * gap);
    vertex->pat.add_point(point);

    if(last != NULL){
        last->add_child(cur_mol.first, vertex);
    }
}


void add_vertices_level_wise(Graph &graph, std::ifstream &myfile,
                             unsigned int total_lines, unsigned int total_cols,
                             int total_mols, int min_group_size){

    HashMolMap mol_map;
    Node *last_vertex[total_cols] = {NULL};
    
    int level_size = graph.level.size();
    double pair_vector[2];

    std::string line, value;
    std::stringstream linestream;

    index_value cur_line[total_cols];
    int patts[total_cols][total_lines - min_group_size];
    // std::cout << total_lines - min_group_size << std::endl;
    // std::cout << total_cols << std::endl;

    // Build first level based on minimum size of rows
    for(int k = 0; k < min_group_size - 1; k++){
        getline(myfile, line);
        std::stringstream linestream(line);

        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<double>(value, ',', pair_vector);
            insert_sorted(patts[point], (int) pair_vector[0], k);
            // print_array(patts[point], k + 1);
        }
    }

    getline(myfile, line);
    linestream = std::stringstream(line);
    for(unsigned int point = 0; point < total_cols; point++){
        getline(linestream, value, ' ');
        split<double>(value, ',', pair_vector);
        cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);
    }

    // for(auto &e: cur_line){
    //     std::cout << e.first << ',' << e.second << ' ';
    // }
    // std::cout << std::endl;
    // std::cout << graph << std::endl;

    // level_size = 2;
    for (int level = 0, cur_index = min_group_size; level < level_size; level++, cur_index++){
        getline(myfile, line);
        std::stringstream linestream(line);
        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<double>(value, ',', pair_vector);

            index_value cur_mol = cur_line[point];
            double next_value = pair_vector[1];

            cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);

            insert_sorted(patts[point], cur_mol.first, cur_index - 1);
            // print_array(patts[point], cur_index);

            Node *vertex = insert_graph(graph, mol_map, patts[point], cur_index, level, total_mols);
            update_vertex(vertex, point, cur_mol, next_value, last_vertex[point]);
            last_vertex[point] = vertex;
        }
        mol_map.clear();
        std::cout << "Finished level " << level << std::endl << std::flush;
    }

    for(auto &level : graph.level){
        for(auto &node : level){
            node.pat.quality = sqrt(node.pat.quality);
            node.pat.best_quality = node.pat.quality;
        }
    }

    std::cout << "Finished updating quality on whole graph" << std::endl << std::flush;
}
void build_graph(Graph &graph, const std::string &filename, int min_group_size){

    if(min_group_size < 1) min_group_size = 1;

    std::ifstream myfile(filename.c_str());

    if (myfile.is_open()){
        std::string line;

        getline(myfile, line);
        std::vector<int> aux = split<std::vector<int> >(line, ',');
        // std::cout << aux.size() << std::endl;
        int total_lines = aux[0];
        int total_cols = aux[1];
        int total_mols = aux[2];


        if(min_group_size > (total_lines - 1)/2 ){
            std::cerr << "Value of k greater than half the number of molecules ("
                 << total_lines << ")." << std::endl;
            exit(0);
        }

        int level_size = total_lines - 2 * min_group_size + 1;
        // std::cout << level_size << std::endl;
        graph.level.resize(level_size);
        add_vertices_level_wise(graph, myfile, total_lines, total_cols, total_mols, min_group_size);
        myfile.close();
    }
    else{
        std::cerr << "Unable to open file";
        exit(0);
    }
}


void level1(Graph &g, std::list<std::pair<int, Pattern>> &sel){
    bool possible;
    int i = 0;
    // First step - going down
    for(auto &level : g.level){
        // std::cout << i << ':';
        for(auto &node : level){
            // std::cout << node.pat.best_quality << '(';
            for(auto &child : node.next){
                // std::cout << child->pat.best_quality << ',';
                child->pat.best_quality = std::max(node.pat.best_quality, child->pat.best_quality);
            }
            // std::cout << ") ";
        }
        // std::cout << std::endl;
        i++;
    }
    int level = g.level.size() - 1;
    // // Second step - going up
    for (auto it = g.level.rbegin(); it != g.level.rend(); ++it) {
        for(auto &node : *it){
            possible = (node.pat.quality >= node.pat.best_quality);
            node.pat.best_quality = node.pat.quality;

            for(auto &child : node.next){
                node.pat.best_quality = std::max(node.pat.best_quality, child->pat.best_quality);
            }

            if(possible && (node.pat.quality == node.pat.best_quality)){
                sel.push_back(std::make_pair(level, node.pat));
                node.pat.best_quality++;
            }
        }

        if(level < (g.level.size() - 1)){
            g.level[level+1].clear();
        }
        level--;
    }
    g.level[0].clear();
}


bool maxcmp (FinalPattern &i, FinalPattern &j) { return (i.quality > j.quality); }


bool is_sub_or_sup(const FinalPattern &pat1, const FinalPattern &pat2){
    auto& small = (pat1.rows.size() > pat2.rows.size()) ? pat2.rows : pat1.rows;
    auto& big = (pat1.rows.size() > pat2.rows.size()) ? pat1.rows : pat2.rows;

    return std::includes(big.begin(), big.end(), small.begin(), small.end());
}


void post_process(std::list<FinalPattern> &selected){

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


