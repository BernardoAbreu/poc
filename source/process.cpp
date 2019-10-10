#include "process.h"
#include <cstdio>
#include <vector>
// #include <boost/progress.hpp>

struct extraction_info{
    int col;
    int total_rows;
    int pat_index;
};


InterPattern* create_pattern(HashMolMap &mol_map, unsigned int *mol_set, unsigned int mol_size, int level){

    InterPattern *vertex;

    std::string key = compress_array<unsigned int>(mol_set, mol_size);
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


void insert_next_element(std::stringstream &linestream, index_value &cur_mol, double &next_value, index_value* cur_elem, unsigned int *patt, int patt_size){
    std::string value;
    double pair_vector[2];
    getline(linestream, value, ' ');
    split<double>(value, ',', pair_vector);
    cur_mol = *cur_elem;
    next_value = pair_vector[1];
    *cur_elem = std::make_pair((int) pair_vector[0], pair_vector[1]);
    insert_sorted(patt, cur_mol.first, patt_size);
}

bool moveToStartOfLine(std::ifstream& fs){
    fs.seekg(-1, std::ios_base::cur);
    for(long i = fs.tellg(); i > 0; i--){
        if(fs.peek() == '\n'){
            fs.get();
            return true;
        }
        fs.seekg(i, std::ios_base::beg);
    }
    return false;
}


void build_last_values(std::ifstream &fs, double *last_value, unsigned int arr_size){
    double pair_vector[2];
    int cur_pos = fs.tellg();
    // Go to the last character before EOF
    fs.seekg(-1, std::ios_base::end);
    if (!moveToStartOfLine(fs))
        return;

    std::string lastline, value;
    getline(fs, lastline);

    std::stringstream linestream(lastline);
    for(unsigned int i = 0; i < arr_size; i++){
        getline(linestream, value, ' ');
        split<double>(value, ',', pair_vector);
        last_value[i] = pair_vector[1];
    }

    fs.seekg(cur_pos);
}


void add_vertices_level_wise(std::ifstream &myfile, int level_size, unsigned int total_lines, unsigned int total_cols, int total_mols, int min_group_size, std::list<std::pair<int, Pattern>> &out_aux){

    HashMolMap mol_map;
    InterPattern *last_vertex[total_cols] = {NULL};
    InterPattern *cur_vertex[total_cols] = {NULL};
    double last_value[total_cols] = {0};

    build_last_values(myfile, last_value, total_cols);

    double pair_vector[2];

    index_value cur_mol;
    double next_value;

    std::string line, value;
    std::stringstream linestream;

    index_value cur_line[total_cols];

    int patts_lines = total_lines - min_group_size;
    unsigned int** patts = new unsigned int*[total_cols];
    for(unsigned int col = 0; col < total_cols; col++){
        patts[col] = new unsigned int[patts_lines];
    }

    // Build minimum size of rows before first level
    for(int k = 0; k < min_group_size - 1; k++){
        getline(myfile, line);
        std::stringstream linestream(line);
        for(unsigned int point = 0; point < total_cols; point++){
            getline(linestream, value, ' ');
            split<double>(value, ',', pair_vector);
            insert_sorted(patts[point], (unsigned int) pair_vector[0], k);
        }
    }

    getline(myfile, line);
    linestream = std::stringstream(line);

    for(unsigned int point = 0; point < total_cols; point++){
        getline(linestream, value, ' ');
        split<double>(value, ',', pair_vector);
        cur_line[point] = std::make_pair((int) pair_vector[0], pair_vector[1]);
    }

    int pattern_count = total_cols;
    // boost::progress_display show_progress(level_size);
    for (int level = 0, cur_index = min_group_size; (pattern_count > 0) && level < level_size; level++, cur_index++){
        getline(myfile, line);
        linestream = std::stringstream(line);
        pattern_count = total_cols;

        if(level != 0){
            for(unsigned int col = 0; col < total_cols; col++){
                last_vertex[col]->count = 0;
            }
        }

        // Sets all local quality values
        for(unsigned int col = 0; col < total_cols; col++){
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

            if(next_value == last_value[col]){
                pattern_count--;
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
        // printf("Level %d/%d - %d\n", level, level_size, pattern_count);
        // fflush(stdout);
        // ++show_progress;
    }
    printf("Finished all levels\n");
    fflush(stdout);

    for(unsigned int col = 0; col < total_cols; col++){
        last_vertex[col]->reset_visited();
        delete[] patts[col];
    }
    delete[] patts;

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


bool operator<(const extraction_info& a, int b){
    return a.col < b;
}


void build_rows(std::ifstream &myfile, int k, std::list<std::pair<int, Pattern>> &out_aux, std::list<Pattern> &out){

    unsigned int row;
    std::list<extraction_info> not_extracted;
    std::vector<Pattern> final_pat(out_aux.size());

    int i = 0;
    for(auto &e: out_aux){
        final_pat[i] = e.second;
        int col = final_pat[i].cols.back();
        not_extracted.insert(
            std::lower_bound(not_extracted.begin(), not_extracted.end(), col),
            {col, e.first + k, i});
        i++;
    }

    std::string line, tuple, value;
    getline(myfile, line);

    for(int i = 0; !not_extracted.empty(); i++){
        auto it = not_extracted.begin();

        getline(myfile, line);
        std::stringstream linestream(line);
        for(int col = 0; it != not_extracted.end(); col++){
            getline(linestream, tuple, ' ');
            if(col == (*it).col){
                getline(std::stringstream(tuple), value, ',');
                std::istringstream(value) >> row;
                insert_sorted(final_pat[(*it).pat_index].rows, row);

                if((*it).total_rows == (i + 1)){
                    it = not_extracted.erase(it);
                }
                else{
                    it++;
                }
            }
        }
    }

    std::copy(final_pat.begin(), final_pat.end(), std::back_inserter(out));
}


void extract_patterns(const std::string &filename, unsigned int min_group_size, std::list<Pattern> &out){

    if(min_group_size < 1) min_group_size = 1;

    std::ifstream myfile(filename.c_str());

    if (myfile.is_open()){
        std::string line;
        std::list<std::pair<int, Pattern>> out_aux;

        getline(myfile, line);
        std::vector<int> aux = split<std::vector<int> >(line, ',');
        unsigned int total_lines = aux[0];
        unsigned int total_cols = aux[1];
        unsigned int total_mols = aux[2];

        if(min_group_size > (total_lines - 1)/2 ){
            // std::cerr << "Value of k greater than half the number of molecules ("
            //      << total_lines << ")." << std::endl;
            fprintf(stderr, "Value of k greater than half the number of molecules (%hu)\n", total_lines);
            myfile.close();
            exit(0);
        }

        int level_size = total_lines - 2 * min_group_size + 1;

        add_vertices_level_wise(myfile, level_size, total_lines, total_cols, total_mols, min_group_size, out_aux);
        printf("Super set of patterns found\n");
        fflush(stdout);

        myfile.seekg (0, myfile.beg);
        build_rows(myfile, min_group_size, out_aux, out);
        printf("Rows of patterns inserted\n");
        fflush(stdout);
        myfile.close();
    }
    else{
        fprintf(stderr, "Unable to open file\n");
        exit(0);
    }
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
