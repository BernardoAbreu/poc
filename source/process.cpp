#include "process.h"
#include <iostream>


Node* insert_graph(Graph &graph, HashMolMap &mol_map, int mol_set[], int mol_size, int level, int total_mols){

    Node *vertex;

    std::string key = compress_array(mol_set, mol_size);

    std::pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

    if(search_result.second){
        Pattern pat(mol_set, mol_size);
        Node n(pat, total_mols);
        vertex = graph.insert(level, n);
        (search_result.first)->second = vertex;
    }
    else{
        vertex = (search_result.first)->second;
    }
    return vertex;
}


Node* insert_graph(Graph &graph, HashMolMap &mol_map, const Pattern& pat, int level, int total_mols){

    Node *vertex;

    std::string key = compress_array(pat.molecules, pat.get_mol_size());

    std::pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

    if(search_result.second){
        Node n(pat, total_mols);
        vertex = graph.insert(level, n);
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
    // std::cout << cur_mol.first << std::endl;
    if(last != NULL){
        // std::cout << *last << std::endl;
        // std::cout << last->children.size() << std::endl;
        last->add_child(cur_mol.first, vertex);
    }
}


void add_vertices_edges_hashed(Graph &graph, index_value **matrix, const std::pair<unsigned int, unsigned int> dimensions, int min_group_size){
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
            double next_value = matrix[i][mol_size].second;

            insert_sorted(mol_set, cur_mol.first, mol_size - 1);
            Node *vertex = insert_graph(graph, mol_map, mol_set, mol_size,
                                        level, dimensions.second);

            update_vertex(vertex, point, cur_mol, next_value, last_vertex);
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

    {
        int old_patts[total_cols][min_group_size];
        // Build first level based on minimum size of rows
        for(int k = 0; k < min_group_size - 1; k++){
            getline(myfile, line);
            std::stringstream linestream(line);

            for(unsigned int point = 0; point < total_cols; point++){
                getline(linestream, value, ' ');
                split<double>(value, ',', pair_vector);
                insert_sorted(old_patts[point], (int) pair_vector[0], k);
            }
        }

        getline(myfile, line);
        linestream = std::stringstream(line);
        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<double>(value, ',', pair_vector);
            cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);
        }

        getline(myfile, line);
        linestream = std::stringstream(line);
        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<double>(value, ',', pair_vector);

            index_value cur_mol = cur_line[point];
            double next_value = pair_vector[1];

            cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);

            insert_sorted(old_patts[point], cur_mol.first, min_group_size - 1);

            Node *vertex = insert_graph(graph, mol_map, old_patts[point], min_group_size , 0, total_mols);
            update_vertex(vertex, point, cur_mol, next_value, last_vertex[point]);
            last_vertex[point] = vertex;
        }
        std::cout << "Finished level 0\n" << std::flush;
    }
    mol_map.clear();

    // std::cout << graph << std::endl;

    for (int level = 1; level < level_size; level++){
        getline(myfile, line);
        std::stringstream linestream(line);
        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<double>(value, ',', pair_vector);

            index_value cur_mol = cur_line[point];
            double next_value = pair_vector[1];

            cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);

            Pattern new_pat(last_vertex[point]->pat, cur_mol.first);

            Node *vertex = insert_graph(graph, mol_map, new_pat, level, total_mols);
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

void add_vertices_level_wise(Graph &graph, index_value **matrix, const std::pair<unsigned int, unsigned int> dimensions, int min_group_size){

    unsigned int total_mols = dimensions.first;
    unsigned int total_points = dimensions.second;
    int level_size = graph.level.size();

    HashMolMap mol_map;
    Node *last_vertex[total_points];
    
    int old_patts[total_points][min_group_size];

    // Build first level based on minimum size of rows
    for(unsigned int point = 0; point < total_points; point++){
        index_value cur_mol;

        for(int k = 0; k < min_group_size - 1; k++){
            cur_mol = matrix[k][point];
            insert_sorted(old_patts[point], cur_mol.first, k);
        }
        last_vertex[point] = NULL;
    }

    std::cout << total_mols << ' ' << total_points << ' ' << min_group_size << std::endl;
    for(int i = 0; i < (min_group_size - 2); i++){
        std::cout << i << std::endl;
        delete[] matrix[i];
    }
    std::cout << min_group_size - 1 << std::endl;

    for(unsigned int point = 0; point < total_points; point++){
        index_value cur_mol = matrix[min_group_size - 1][point];
        double next_value = matrix[min_group_size][point].second;
        insert_sorted(old_patts[point], cur_mol.first, min_group_size - 1);

        Node *vertex = insert_graph(graph, mol_map, old_patts[point], min_group_size , 0, total_mols);
        // std::cout << dimensions.second << std::endl;
        update_vertex(vertex, point, cur_mol, next_value, last_vertex[point]);
        last_vertex[point] = vertex;
    }
    std::cout << graph << std::endl;

    mol_map.clear();
    // std::cout << min_group_size - 1 << std::endl;
    delete[] matrix[min_group_size - 1];

    // std::cout << dimensions.first << std::endl;
    for (int level = 1; level < level_size; level++){
        int mol_size = level + min_group_size;
        // std::cout << mol_size << std::endl;

        for(unsigned int point = 0; point < total_points; point++){
            index_value cur_mol = matrix[mol_size - 1][point];
            double next_value = matrix[mol_size][point].second;

            Pattern new_pat(last_vertex[point]->pat, cur_mol.first);
            // std::cout << new_pat << std::endl;
            Node *vertex = insert_graph(graph, mol_map, new_pat, level, total_mols);
            // std::cout << level << ' ' << point << ' ' << cur_mol.first << std::endl;
            update_vertex(vertex, point, cur_mol, next_value, last_vertex[point]);
            last_vertex[point] = vertex;

        }
        mol_map.clear();
        // std::cout << mol_size - 1 << std::endl;
        delete[] matrix[mol_size - 1];
    }

    for(unsigned int i = level_size + min_group_size - 1; i < dimensions.first; i++){
        // std::cout << i << std::endl;
        delete[] matrix[i];
    }


    for(auto &level : graph.level){
        for(auto &node : level){
            node.pat.quality = sqrt(node.pat.quality);
            node.pat.best_quality = node.pat.quality;
        }
    }
}


void build_graph(Graph &graph, index_value **matrix, const std::pair<unsigned int, unsigned int> &dimensions, int min_group_size){

    if(min_group_size < 1) min_group_size = 1;

    int level_size = dimensions.first - 2 * min_group_size + 1;
    // std::cout << dimensions.first << ' ' << dimensions.second << ' ' << level_size << std::endl;

    graph.level.resize(level_size);
    add_vertices_level_wise(graph, matrix, dimensions, min_group_size);
}


void build_graph(Graph &graph, const std::string &filename, int min_group_size){

    if(min_group_size < 1) min_group_size = 1;

    std::ifstream myfile(filename.c_str());

    if (myfile.is_open()){
        std::string line;

        getline(myfile, line);
        std::vector<int> aux = split<std::vector<int> >(line, ',');
        int total_lines = aux[0];
        int total_cols = aux[1];
        int total_mols = aux[2];


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
    }
}


// void build_graph(Graph &graph, index_value **matrix, const std::pair<unsigned int, unsigned int> &dimensions, unsigned int min_group_size){

//     if(min_group_size < 1) min_group_size = 1;

//     int level_size = dimensions.second - 2 * min_group_size + 1;

//     graph.level.resize(level_size);
//     add_vertices_edges_hashed(graph, matrix, dimensions, min_group_size);
// }


void build_graph_from_file(Graph &graph, const std::string &filename, int min_group_size){

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
                double next_value = fixed_row[mol_size].second;

                insert_sorted(mol_set, cur_mol.first, mol_size - 1);

                Node *vertex = insert_graph(graph, mol_map, mol_set, mol_size,
                                            level, total_mols);

                update_vertex(vertex, point, cur_mol, next_value, last_vertex);

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


