#include "process.h"
#include <iostream>
#include <vector>

InterPattern* create_pattern(HashMolMap &mol_map, unsigned short int *mol_set,
                             unsigned short int mol_size, int level){

    InterPattern *vertex;

    std::string key = compress_array<unsigned short int>(mol_set, mol_size);
    std::pair<HashMolMap::iterator, bool> search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

    if(search_result.second){
        vertex = new InterPattern(level);
        (search_result.first)->second = vertex;
    }
    else{
        vertex = (search_result.first)->second;
    }
    return vertex;
}


void insert_next_element(std::stringstream &linestream, index_value &cur_mol,
                         double &next_value, index_value* cur_elem,
                         unsigned short int *patt, int patt_size){
    std::string value;
    double pair_vector[2];
    getline(linestream, value, ' ');
    split<double>(value, ',', pair_vector);
    cur_mol = *cur_elem;
    next_value = pair_vector[1];
    *cur_elem = std::make_pair((int) pair_vector[0], pair_vector[1]);
    insert_sorted(patt, cur_mol.first, patt_size);
}


void add_vertices_level_wise(std::ifstream &myfile, int level_size,
                             unsigned int total_lines, unsigned int total_cols,
                             int total_mols, int min_group_size,
                             std::list<std::pair<int, Pattern>> &out_aux){

    HashMolMap mol_map;
    InterPattern *last_vertex[total_cols] = {NULL};
    InterPattern *cur_vertex[total_cols] = {NULL};

    double pair_vector[2];

    index_value cur_mol;
    double next_value;

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
            split<double>(value, ',', pair_vector);
            insert_sorted(patts[point], (unsigned short int) pair_vector[0], k);
        }
    }

    getline(myfile, line);
    linestream = std::stringstream(line);

    for(unsigned int point = 0; point < total_cols; point++){
        getline(linestream, value, ' ');
        split<double>(value, ',', pair_vector);
        cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);
    }

    for (int level = 0, cur_index = min_group_size; level < level_size; level++, cur_index++){
        getline(myfile, line);
        linestream = std::stringstream(line);

        if(level != 0){
            for(unsigned int col = 0; col < total_cols; col++){
                last_vertex[col]->count = 0;
            }
        }

        // Sets all local quality values
        for(unsigned short int col = 0; col < total_cols; col++){
            insert_next_element(linestream, cur_mol, next_value, &(cur_line[col]),
                                patts[col], cur_index - 1);

            cur_vertex[col] = create_pattern(mol_map, patts[col], cur_index, level);

            double gap = std::abs(cur_mol.second - next_value);
            cur_vertex[col]->quality += (gap * gap);
            cur_vertex[col]->add_col(col);

            cur_vertex[col]->reset_visited();
            cur_vertex[col]->count++;

            if(last_vertex[col] != NULL){
                last_vertex[col]->count++;
                last_vertex[col]->reset_visited();
            }
        }

        // Sets all global best quality values
        for(unsigned int col = 0; col < total_cols; col++){
            // Update quality according to the defined norm
            if(!cur_vertex[col]->is_visited()){
                cur_vertex[col]->quality = sqrt(cur_vertex[col]->quality);
                cur_vertex[col]->set_visited();
            }
            if(last_vertex[col] != NULL){
                if(cur_vertex[col]->quality >= last_vertex[col]->quality){
                    if(!last_vertex[col]->is_visited()){
                        last_vertex[col]->count--;
                    }
                    if(last_vertex[col]->count > 0){
                        last_vertex[col]->set_invalid();
                    }
                    else{
                        delete last_vertex[col];
                    }

                    last_vertex[col] = cur_vertex[col];
                }
                else{
                    cur_vertex[col]->count--;
                    if(cur_vertex[col]->count > 0){
                        cur_vertex[col]->set_invalid();
                    }
                    else{
                        delete cur_vertex[col];
                    }

                    if(last_vertex[col]->is_visited()){
                        last_vertex[col]->count++;
                    }
                    else{
                        last_vertex[col]->set_visited();
                        last_vertex[col]->count = 1;
                    }
                }
            }
            else{
                last_vertex[col] = cur_vertex[col];
            }
        }
        mol_map.clear();
        std::cout << "Level " << level << '/' << level_size << std::endl << std::flush;
    }
    std::cout << "Finished all levels" << std::endl << std::flush;

    for(unsigned int col = 0; col < total_cols; col++){
        last_vertex[col]->reset_visited();
        delete[] patts[col];
    }
    delete[] patts;

    std::cout << "Finished updating quality on whole graph" << std::endl << std::flush;

    for(unsigned int col = 0; col < total_cols; col++){
        if(!last_vertex[col]->is_visited() && !last_vertex[col]->is_invalid()){
            out_aux.push_back(std::make_pair(last_vertex[col]->row_index,
                    Pattern(last_vertex[col]->quality, last_vertex[col]->cols)));
            last_vertex[col]->set_visited();
        }
        last_vertex[col]->count--;
        if(last_vertex[col]->count == 0){
            delete last_vertex[col];
        }
    }
}


void build_graph(const std::string &filename, int min_group_size, std::list<std::pair<int, Pattern>> &out_aux){

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

        add_vertices_level_wise(myfile, level_size, total_lines, total_cols, total_mols, min_group_size, out_aux);
        myfile.close();
    }
    else{
        std::cerr << "Unable to open file\n";
        exit(0);
    }
}


// void level1(Graph &g, std::list<std::pair<int, Pattern>> &sel){
//     bool possible;

//     // First step - going down
//     for(auto &level : g.level){
//         for(auto &node : level){
//             for(auto &child : node.next){
//                 child->best_quality = std::max(node.best_quality, child->best_quality);
//             }
//             if(node.quality != node.best_quality){
//                 node.points.clear();
//                 node.children.clear();
//             }
//         }
//     }

//     unsigned int level = g.level.size() - 1;
//     // Second step - going up
//     for (auto it = g.level.rbegin(); it != g.level.rend(); ++it) {
//         for(auto &node : *it){
//             possible = (node.quality >= node.best_quality);
//             node.best_quality = node.quality;

//             for(auto &child : node.next){
//                 node.best_quality = std::max(node.best_quality, child->best_quality);
//             }

//             if(possible && (node.quality == node.best_quality)){
//                 sel.push_back(std::make_pair(level, Pattern(node.quality, node.points)));
//                 node.best_quality++;
//             }
//             else{
//                 node.points.clear();
//                 node.children.clear();
//                 node.next.clear();
//             }
//         }

//         if(level < (g.level.size() - 1)){
//             g.level[level+1].clear();
//         }
//         level--;
//     }
//     g.level[0].clear();
// }


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
