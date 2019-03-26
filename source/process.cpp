#include "process.h"
#include <iostream>
#include <vector>


Node* insert_graph(Graph &graph, HashMolMap &mol_map,
                   unsigned short int *mol_set, unsigned short int mol_size,
                   int level, unsigned short int total_mols){

    Node *vertex;

    std::string key = compress_array<unsigned short int>(mol_set, mol_size);
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


void update_vertex(Node *vertex, int point, index_value &cur_mol, float next_value, Node *last){

    float gap = std::abs(cur_mol.second - next_value);

    // vertex->add_gap(gap * gap);
    vertex->add_gap(gap);
    vertex->add_point(point);

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
    float pair_vector[2];

    std::string line, value;
    std::stringstream linestream;

    index_value cur_line[total_cols];

    int patts_lines = total_lines - min_group_size;
    unsigned short int** patts = new unsigned short int*[total_cols];
    for(unsigned int point = 0; point < total_cols; point++){
        patts[point] = new unsigned short int[patts_lines];
    }

    // Build minimum size of rows before first level
    for(int k = 0; k < min_group_size - 1; k++){
        getline(myfile, line);
        std::stringstream linestream(line);
        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<float>(value, ',', pair_vector);
            insert_sorted(patts[point], (unsigned short int) pair_vector[0], k);
        }
    }

    getline(myfile, line);
    linestream = std::stringstream(line);

    for(unsigned int point = 0; point < total_cols; point++){
        getline(linestream, value, ' ');
        split<float>(value, ',', pair_vector);
        cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);
    }

    for (int level = 0, cur_index = min_group_size; level < level_size; level++, cur_index++){
        getline(myfile, line);
        linestream = std::stringstream(line);
        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<float>(value, ',', pair_vector);

            index_value cur_mol = cur_line[point];
            double next_value = pair_vector[1];

            cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);

            insert_sorted(patts[point], cur_mol.first, cur_index - 1);

            Node *vertex = insert_graph(graph, mol_map, patts[point], cur_index, level, total_mols);
            update_vertex(vertex, point, cur_mol, next_value, last_vertex[point]);
            last_vertex[point] = vertex;
        }
        mol_map.clear();
        std::cout << "Level " << level << '/' << level_size << std::endl << std::flush;
    }

    std::cout << "Finished all levels" << std::endl << std::flush;

    for(unsigned int point = 0; point < total_cols; point++){
        delete[] patts[point];
    }
    delete[] patts;

    // for(auto &level : graph.level){
    //     for(auto &node : level){
    //         node.quality = sqrt(node.quality);
    //         node.best_quality = node.quality;
    //     }
    // }

    std::cout << "Finished updating quality on whole graph" << std::endl << std::flush;
}


void build_graph(Graph &graph, const std::string &filename, int min_group_size){

    if(min_group_size < 1) min_group_size = 1;

    std::ifstream myfile(filename.c_str());

    if (myfile.is_open()){
        std::string line;

        getline(myfile, line);
        std::vector<int> aux = split<std::vector<int> >(line, ',');
        unsigned short int total_lines = aux[0];
        unsigned short int total_cols = aux[1];
        unsigned short int total_mols = aux[2];

        if(min_group_size > (total_lines - 1)/2 ){
            std::cerr << "Value of k greater than half the number of molecules ("
                 << total_lines << ")." << std::endl;
            exit(0);
        }

        int level_size = total_lines - 2 * min_group_size + 1;

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

    // First step - going down
    for(auto &level : g.level){
        for(auto &node : level){
            for(auto &child : node.next){
                child->best_quality = std::max(node.best_quality, child->best_quality);
            }
            if(node.quality != node.best_quality){
                node.points.clear();
                node.children.clear();
            }
        }
    }

    unsigned int level = g.level.size() - 1;
    // Second step - going up
    for (auto it = g.level.rbegin(); it != g.level.rend(); ++it) {
        for(auto &node : *it){
            possible = (node.quality >= node.best_quality);
            node.best_quality = node.quality;

            for(auto &child : node.next){
                node.best_quality = std::max(node.best_quality, child->best_quality);
            }

            if(possible && (node.quality == node.best_quality)){
                sel.push_back(std::make_pair(level, Pattern(node.quality, node.points)));
                node.best_quality++;
            }
            else{
                node.points.clear();
                node.children.clear();
                node.next.clear();
            }
        }

        if(level < (g.level.size() - 1)){
            g.level[level+1].clear();
        }
        level--;
    }
    g.level[0].clear();
}


bool maxcmp (Pattern &i, Pattern &j) { return (i.quality > j.quality); }


bool is_sub_or_sup(const Pattern &pat1, const Pattern &pat2){
    auto& small = (pat1.rows.size() > pat2.rows.size()) ? pat2.rows : pat1.rows;
    auto& big = (pat1.rows.size() > pat2.rows.size()) ? pat1.rows : pat2.rows;

    return std::includes(big.begin(), big.end(), small.begin(), small.end());
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
