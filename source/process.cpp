#include "process.h"
#include <cstdio>
#include <vector>
// #include <boost/progress.hpp>

struct extraction_info{
    int col;
    unsigned int total_rows;
    int pat_index;
};


bool operator<(const extraction_info& a, int b) { return a.col < b; }


bool maxcmp (Pattern &i, Pattern &j) { return (i.quality > j.quality); }


bool is_sub_or_sup(const Pattern &pat1, const Pattern &pat2){
    auto& small = (pat1.rows.size() > pat2.rows.size()) ? pat2.rows : pat1.rows;
    auto& big = (pat1.rows.size() > pat2.rows.size()) ? pat1.rows : pat2.rows;

    return std::includes(big.begin(), big.end(), small.begin(), small.end());
}


InterPattern* create_pattern(HashMolMap &row_map, unsigned int *row_set, unsigned int row_size, int level){
    InterPattern *vertex;

    std::string key = compress_array<unsigned int>(row_set, row_size);
    std::pair<HashMolMap::iterator, bool> search_result = row_map.insert(HashMolMap::value_type(key, NULL));

    if (search_result.second){
        vertex = new InterPattern(level);
        (search_result.first)->second = vertex;
    }
    else{
        vertex = (search_result.first)->second;
    }

    return vertex;
}


bool moveToStartOfLine(std::ifstream& fs){
    fs.seekg(-1, std::ios_base::cur);
    for (long i = fs.tellg(); i > 0; i--){
        if (fs.peek() == '\n'){
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
    for (unsigned int i = 0; i < arr_size; i++){
        getline(linestream, value, ' ');
        split<double>(value, ',', pair_vector);
        last_value[i] = pair_vector[1];
    }

    fs.seekg(cur_pos);
}


void insert_next_element(std::stringstream &linestream, index_value &cur_row, double &next_value, index_value* cur_elem, unsigned int *patt, int patt_size){
    std::string value;
    double pair_vector[2];
    getline(linestream, value, ' ');
    split<double>(value, ',', pair_vector);
    cur_row = *cur_elem;
    next_value = pair_vector[1];
    *cur_elem = std::make_pair((int) pair_vector[0], pair_vector[1]);
    insert_sorted(patt, cur_row.first, patt_size);
}


double set_local_quality(std::stringstream &linestream, int level, int cur_index, int col, unsigned int *patt, HashMolMap &row_map, index_value* cur_elem, InterPattern *last_vertex, InterPattern **cur_vertex){
    index_value cur_row;
    double next_value;
    insert_next_element(linestream, cur_row, next_value, cur_elem, patt, cur_index - 1);

    *cur_vertex = create_pattern(row_map, patt, cur_index, level);

    double gap = std::abs(cur_row.second - next_value);
    (*cur_vertex)->quality += (gap * gap);
    (*cur_vertex)->add_col(col);

    (*cur_vertex)->count++;

    return next_value;
}


void set_global_quality(InterPattern **cur_vertex, InterPattern **last_vertex){
    // Update quality according to the defined norm
    if (!(*cur_vertex)->is_visited()){
        (*cur_vertex)->quality = sqrt((*cur_vertex)->quality);
        (*cur_vertex)->set_visited();
    }

    if ((*last_vertex) != NULL){
        if ((*cur_vertex)->quality >= (*last_vertex)->quality){

            if ((*cur_vertex)->quality == (*last_vertex)->quality){
                (*cur_vertex)->set_tied();
            }

            if (!(*last_vertex)->is_visited()){
                (*last_vertex)->count--;
            }

            if ((*last_vertex)->count > 0){
                (*last_vertex)->set_invalid();
            }
            else{
                delete (*last_vertex);
            }

            (*last_vertex) = (*cur_vertex);
        }
        else{
            (*cur_vertex)->count--;
            if ((*cur_vertex)->count > 0){
                (*cur_vertex)->set_invalid();
            }
            else{
                delete (*cur_vertex);
            }

            if ((*last_vertex)->is_visited()){
                (*last_vertex)->count++;
            }
            else{
                (*last_vertex)->set_visited();
                (*last_vertex)->count = 1;
            }
        }
    }
    else{
        (*last_vertex) = (*cur_vertex);
    }
}


int add_level(std::ifstream &myfile, int level, int cur_index, unsigned int total_cols, unsigned int** patts, index_value cur_line[], double *last_value, InterPattern *last_vertex[], InterPattern *cur_vertex[]){

    bool has_new_pattern = false;
    double next_value;
    std::string line;

    HashMolMap row_map;

    getline(myfile, line);
    std::stringstream linestream = std::stringstream(line);

    if (level != 0){
        for (unsigned int col = 0; col < total_cols; col++){
            last_vertex[col]->count = 0;
        }
    }

    // Sets all local quality values
    for (unsigned int col = 0; col < total_cols; col++){
        next_value = set_local_quality(linestream, level, cur_index, col,
                          patts[col], row_map, &(cur_line[col]), &(cur_vertex[col]));

        if (last_vertex[col] != NULL){
            last_vertex[col]->count++;
            last_vertex[col]->reset_visited();

            if (next_value != last_value[col]){
                has_new_pattern = true;
            }
        }
    }

    // Sets all global best quality values
    for (unsigned int col = 0; col < total_cols; col++){
        set_global_quality(&(cur_vertex[col]), &(last_vertex[col]));
    }

    return has_new_pattern;
}


void add_vertices_level_wise(std::ifstream &myfile, int level_size, unsigned int total_lines, unsigned int total_cols, int min_group_size, std::list<std::pair<unsigned int, Pattern>> &out_aux){

    InterPattern *last_vertex[total_cols] = {NULL};
    InterPattern *cur_vertex[total_cols] = {NULL};

    double pair_vector[2];
    double last_value[total_cols] = {0};

    build_last_values(myfile, last_value, total_cols);

    index_value cur_line[total_cols];

    std::string line, value;
    std::stringstream linestream;

    int patts_lines = total_lines - min_group_size;
    unsigned int** patts = new unsigned int*[total_cols];
    for (unsigned int col = 0; col < total_cols; col++){
        patts[col] = new unsigned int[patts_lines];
    }

    // Build minimum size of rows before first level
    for (int k = 0; k < min_group_size - 1; k++){
        getline(myfile, line);
        linestream = std::stringstream(line);
        for (unsigned int col = 0; col < total_cols; col++){
            getline(linestream, value, ' ');
            split<double>(value, ',', pair_vector);
            insert_sorted(patts[col], (unsigned int) pair_vector[0], k);
        }
    }

    getline(myfile, line);
    linestream = std::stringstream(line);
    for (unsigned int col = 0; col < total_cols; col++){
        getline(linestream, value, ' ');
        split<double>(value, ',', pair_vector);
        cur_line[col] = std::make_pair((int) pair_vector[0], pair_vector[1]);
    }

    // boost::progress_display show_progress(level_size);
    for (int level = 0, cur_index = min_group_size;level < level_size; level++, cur_index++){
        if (!add_level(myfile, level, cur_index, total_cols, patts,
                       cur_line, last_value, last_vertex, cur_vertex)){
            break;
        }

        // printf("Level %d/%d - %d\n", level, level_size, pattern_count);
        // fflush(stdout);
        // ++show_progress;
    }
    printf("Finished all levels\n");
    fflush(stdout);

    for (unsigned int col = 0; col < total_cols; col++){
        last_vertex[col]->reset_visited();
        delete[] patts[col];
    }
    delete[] patts;

    for (unsigned int col = 0; col < total_cols; col++){
        if (!last_vertex[col]->is_visited() && !last_vertex[col]->is_invalid()){
            out_aux.push_back(
                std::make_pair(
                    last_vertex[col]->row_index,
                    Pattern(last_vertex[col]->quality, last_vertex[col]->cols, last_vertex[col]->is_tied())
                )
            );

            last_vertex[col]->set_visited();
        }

        last_vertex[col]->count--;
        if (last_vertex[col]->count == 0){
            delete last_vertex[col];
        }
    }
}


void build_rows(std::ifstream &myfile, int k, std::list<std::pair<unsigned int, Pattern>> &out_aux, std::list<Pattern> &out){

    unsigned int row;
    std::list<extraction_info> not_extracted;
    std::vector<Pattern> final_pat(out_aux.size());

    int i = 0;
    for (auto &e : out_aux){
        final_pat[i] = e.second;
        int col = final_pat[i].cols.back();
        not_extracted.insert(
            std::lower_bound(not_extracted.begin(), not_extracted.end(), col),
            {col, e.first + k, i});
        i++;
    }

    std::string line, tuple, value;
    getline(myfile, line);

    for (unsigned int i = 0; !not_extracted.empty(); i++){
        getline(myfile, line);
        std::stringstream linestream(line);
        int col = 0;
        for (auto it = not_extracted.begin(); it != not_extracted.end();){
            getline(linestream, tuple, ' ');

            if (col == (*it).col){
                getline(std::stringstream(tuple), value, ',');
                std::istringstream(value) >> row;
                insert_sorted(final_pat[(*it).pat_index].rows, row);

                if ((*it).total_rows == (i + 1)){
                    it = not_extracted.erase(it);
                }
                else{
                    it++;
                }
            }

            col++;
        }
    }

    std::copy(final_pat.begin(), final_pat.end(), std::back_inserter(out));
}


void __extract_patterns_from_file(std::ifstream &myfile, unsigned int total_lines, unsigned int total_cols, unsigned int min_group_size, std::list<Pattern> &out){
    int level_size = total_lines - 2 * min_group_size + 1;
    std::list<std::pair<unsigned int, Pattern>> out_aux;

    add_vertices_level_wise(myfile, level_size, total_lines, total_cols, min_group_size, out_aux);
    printf("Super set of patterns found\n");
    fflush(stdout);

    myfile.seekg(0, myfile.beg);
    build_rows(myfile, min_group_size, out_aux, out);
    printf("Rows of patterns inserted\n");
    fflush(stdout);
}


void extract_patterns(const std::string &filename, unsigned int min_group_size, std::list<Pattern> &out){

    if (min_group_size < 1) min_group_size = 1;

    std::ifstream myfile(filename.c_str());

    if (myfile.is_open()){
        std::string line;

        getline(myfile, line);
        std::vector<unsigned int> matrix_shape = split<std::vector<unsigned int> >(line, ',');

        if (min_group_size > (matrix_shape[0] - 1)/2 ){
            fprintf(stderr, "Value of k greater than half the number of molecules (%hu)\n", matrix_shape[0]);
            myfile.close();
            exit(0);
        }

        __extract_patterns_from_file(myfile, matrix_shape[0], matrix_shape[1], min_group_size, out);
        myfile.close();
    }
    else{
        fprintf(stderr, "Unable to open file\n");
        exit(0);
    }
}


void post_process(std::list<Pattern> &selected){

    selected.sort(maxcmp);

    std::vector<bool> not_valid(selected.size());

    int i = 0;
    for (auto it = selected.begin(); it != std::prev(selected.end()); ++it, i++){
        int j = i + 1;
        for (auto jt = std::next(it); jt != selected.end(); ++jt, j++){
            if (!not_valid[j]){
                not_valid[j] = (*jt).tied || is_sub_or_sup(*it, *jt);
            }
        }
    }

    i = 0;
    for (auto it = selected.begin(); it != selected.end(); i++){
        if (not_valid[i]){
            it = selected.erase(it);
        }
        else{
            it++;
        }
    }
}
