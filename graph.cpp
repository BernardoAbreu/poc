#include <algorithm>

#include <sstream>
#include <set>
#include <math.h>
#include "graph.h"

#include "util.cpp"
#include <iostream>
using namespace std;


template< typename T >
typename std::vector<T>::iterator insert_sorted( std::vector<T> & vec, T const& item ){
    return vec.insert(std::upper_bound( vec.begin(), vec.end(), item ), item);
}


template< typename T >
void insert_sorted( std::vector<T> & vec, T const& item, int const size){
    T item_replace = item;
    for(int i = 0; i < size-1; i++){
        if(vec[i] > item_replace){
            swap(vec[i], item_replace);
        }
    }
    vec[size-1] = item_replace;
}


void level_traverse(graph *g){
    vector<list<node> >::iterator it;
    list<node>::iterator jt;

    int i = 0;

    for (it=g->level.begin(); it != g->level.end(); ++it){
        cout << "Level " << i++ << ':';
        for (jt=(*it).begin(); jt != (*it).end(); ++jt){
            cout << ' ' << (*jt).pat.mol_set << '(' << (*jt).pat.quality << ',' << (*jt).pat.best_quality << ')';

            for (list<node*>::iterator kt=(*jt).next.begin(); kt != (*jt).next.end(); ++kt){
                cout << '>' << (**kt).pat.mol_set;
            }
            cout << '[' << join((*jt).pat.points, ',') << ']';

        }
        cout << endl;
    }

}


// void add_vertices_edges(graph *g, vector<vector<mol_info> > *points){
//     int level_size = g->level.size();
//     vector<int> mol_set(level_size);
//     MolMap mol_map;
//     MolMap::iterator lb;

//     string key;
//     int gap, current_mol;

//     node *last, *np;
//     node n;

//     int points_size = points->size();

//     for (int i = 0; i < points_size; i++){

//         last = NULL;
//         for (int level = 0; level < level_size; level++){
//             current_mol = (*points)[i][level].first;
//             mol_set[level] = current_mol;
//             gap = (*points)[i][level].second - (*points)[i][level+1].second;

//             sort(mol_set.begin(), mol_set.begin()+level+1);

//             key = join(mol_set, ',', level+1);

//             lb = mol_map.lower_bound(key);

//             if(lb != mol_map.end() && !(mol_map.key_comp()(key, lb->first))){
//             //     // key already exists
//                 (lb->second)->quality += gap;
//                 np = lb->second;
//             }
//             else{
//             //     // the key does not exist in the map
//                 n.mol_set = key;
//                 n.quality = gap;
//                 n.best_quality = gap;
//                 g->level[level].push_back(n);
//                 np = &(g->level[level].back());
//                 np->molecules = vector<int>(mol_set.begin(), mol_set.begin()+level+1);

//                 for(int k = 0; k < 4; k++){
//                     np->children.push_back(0);
//                 }

//                 mol_map.insert(lb, MolMap::value_type(key, np));    // Use lb as a hint to insert,
//                                                                 // so it can avoid another lookup
//             }
//             np->points.push_back(i);
//             if(last != NULL){
//                 if(!((last->children[current_mol>>5]>>(current_mol%32))&1)){
//                     last->children[current_mol>>5] |= 1<<(current_mol%32);
//                     last->next.push_back(np);
//                 }
//             }
//             last = np;
//         }
//     }
// }

// void add_vertices_edges_hashed(graph *g, vector<vector<mol_info> > *points){
//     int level_size = g->level.size();
//     vector<int> mol_set(level_size);

//     string key;
//     int gap, current_mol, points_size;

//     node *last, *np;
//     node n;

//     HashMolMap mol_map;

//     points_size = points->size();
//     pair<HashMolMap::iterator, bool> search_result;
//     for (int i = 0; i < points_size; i++){
//         last = NULL;
//         // vector<int> mol_set;
//         for (int level = 0; level < level_size; level++){
//             current_mol = (*points)[i][level].first;
//             gap = (*points)[i][level].second - (*points)[i][level+1].second;

//             // mol_set[level] = current_mol;
//             // sort(mol_set.begin(),mol_set.begin()+level+1);
//             // mol_set.insert(std::upper_bound(mol_set.begin(),mol_set.begin()+level+1, current_mol ), current_mol);
//             insert_sorted(mol_set, current_mol, level+1);

//             key = join(mol_set, ',', level+1);
//             // key = join(mol_set, ',');

//             search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

//             // cout << key << ':' << search_result.second<<endl;
//             if(search_result.second){
//                 n.mol_set = key;
//                 n.quality = gap;
//                 n.best_quality = gap;
//                 g->level[level].push_back(n);
//                 np = &(g->level[level].back());
//                 np->molecules = vector<int>(mol_set.begin(), mol_set.begin()+level+1);

//                 for(int k = 0; k < 4; k++){
//                     np->children.push_back(0);
//                 }

//                 (search_result.first)->second = np;
//             }
//             else{
//                 (search_result.first)->second->quality += gap;
//                 (search_result.first)->second->best_quality += gap;
//                 np = (search_result.first)->second;
//             }
//             np->points.push_back(i);
//             if(last != NULL ){
//                 // cout << last->mol_set << '>' << np->mol_set << ' ' << (current_mol>>5) << ' ' << (current_mol&31) << endl;
//                 if(!((last->children[current_mol>>5]>>(current_mol%32))&1)){
//                     last->children[current_mol>>5] |= 1<<(current_mol & 31);
//                     // cout << "Current:" << last->mol_set << '>' << np->mol_set << endl;
//                     last->next.push_back(np);
//                 }
//             }
//             last = np;
//         }
//     }
// }


void insert_map(int point, graph *g, vector<vector<mol_info> > *points, 
    HashMolMap *mol_map, vector<int> *mol_set, string key, 
    int current_mol, int level, int min_group_size, node **last){

    node *np;
    node n;
    pattern pat;

    pair<HashMolMap::iterator, bool> search_result;
    int gap = (*points)[point][level].second - (*points)[point][level+1].second;
    // cout << "Insert_map:" << key << ' ' << level << ' ' << min_group_size << endl;
    search_result = mol_map->insert(HashMolMap::value_type(key, NULL));

    // cout << key << ':' << search_result.second<<endl;
    if(search_result.second){
        // cout << "Key " << key << " does not exist" << endl;
        pat.mol_set = key;
        pat.quality = 0;
        pat.best_quality = 0;
        pat.gap = 0;
        n.pat = pat;

        g->level[level-min_group_size + 1].push_back(n);

        np = &(g->level[level - min_group_size + 1].back());
        // np->molecules = vector<int>(mol_set->begin(), mol_set->begin()+level+min_group_size);
        np->pat.molecules = vector<int>(mol_set->begin(), mol_set->begin()+level+min_group_size);

        for(int k = 0; k < 4; k++){
            np->children.push_back(0);
        }

        (search_result.first)->second = np;
    }
    else{
        // cout << "Key " << key << " exists" << endl;
        np = (search_result.first)->second;
    }

    np->pat.quality += (gap*gap);

    np->pat.gap += gap;

    np->pat.points.push_back(point);


    if((*last) != NULL ){
        // cout << last->mol_set << '>' << np->mol_set << ' ' << (current_mol>>5) << ' ' << (current_mol&31) << endl;
        if(!(((*last)->children[current_mol>>5]>>(current_mol%32))&1)){
            (*last)->children[current_mol>>5] |= 1<<(current_mol & 31);
            // cout << "Current:" << last->mol_set << '>' << np->mol_set << endl;
            (*last)->next.push_back(np);
        }
    }
    (*last) = np;

}

void apply_sqrt(graph *g){
    vector<list<node> >::iterator it;
    list<node>::iterator jt;

    for (it=g->level.begin(); it != g->level.end(); ++it){
        for (jt=(*it).begin(); jt != (*it).end(); ++jt){
            (*jt).pat.quality = sqrt((*jt).pat.quality);
            (*jt).pat.best_quality = (*jt).pat.quality;
        }
    }
}

void add_vertices_edges_hashed(graph *g, vector<vector<mol_info> > *points, int min_group_size){
    int level_size = g->level.size();

    if(min_group_size < 1) min_group_size = 1;

    vector<int> mol_set(level_size+min_group_size);
    // cout << mol_set.size() <<endl;

    string key;
    int current_mol, points_size;

    node *last;
    node n;

    HashMolMap mol_map;

    points_size = points->size();
    pair<HashMolMap::iterator, bool> search_result;
    for (int i = 0; i < points_size; i++){
        last = NULL;

        for(int k = 0; k < min_group_size; k++){
            current_mol = (*points)[i][k].first;
            insert_sorted(mol_set, current_mol, k+1);
        }
        key = join(mol_set, ',', min_group_size);
        insert_map(i, g, points, &mol_map, &mol_set, key, current_mol, min_group_size-1, min_group_size, &last);

        for (int level = min_group_size; level < level_size ; level++){
            current_mol = (*points)[i][level].first;
            insert_sorted(mol_set, current_mol, level+1);
            key = join(mol_set, ',', level+1);
            insert_map(i, g, points, &mol_map, &mol_set, key, current_mol, level, min_group_size, &last);
        }
    }

    apply_sqrt(g);
}


// void add_vertices(graph *g, vector<vector<mol_info> > *points, MolMap *mol_map){
//     int level_size = g->level.size();
//     vector<int> mol_set(level_size);

//     MolMap::iterator lb;

//     string key;
//     int gap;

//     node *np;
//     node n;

//     int points_size = points->size();
//     for (int i = 0; i < points_size; i++){

//         for (int level = 0; level < level_size; level++){
//             mol_set[level] = (*points)[i][level].first;
//             gap = (*points)[i][level].second - (*points)[i][level+1].second;

//             sort(mol_set.begin(),mol_set.begin()+level+1);

//             key = join(mol_set, ',', level+1);

//             lb = mol_map->lower_bound(key);

//             if(lb != mol_map->end() && !(mol_map->key_comp()(key, lb->first))){
//                 // key already exists
//                 (lb->second)->quality += gap;
//             }
//             else{
//                 // the key does not exist in the map

//                 n.mol_set = key;
//                 n.quality = gap;
//                 n.best_quality = gap;
//                 g->level[level].push_back(n);
//                 np = &(g->level[level].back());
//                 // add_node(g, key, level, gap, np);
//                 np->pat.molecules = vector<int>(mol_set.begin(), mol_set.begin()+level+1);

//                 mol_map->insert(lb, MolMap::value_type(key, np));    // Use lb as a hint to insert,
//                                                                 // so it can avoid another lookup
//             }
//         }
//     }
// }

// void add_edges(graph *g, MolMap *mol_map){
//     int level_size = g->level.size();
//     node *current;
//     string key;
//     MolMap::iterator lb;

//     for (int i = 0; i < level_size; i++){
//         for (list<node>::iterator it=g->level[i].begin(); it != g->level[i].end(); ++it){
//             current = &(*it);

//             for(int removed = 0; removed < i+1; removed++){
//                 key = join(current->molecules, ',', i,removed);

//                 lb = mol_map->find(key);

//                 if (lb != mol_map->end()){
//                     lb->second->next.push_back(current);
//                 }
//             }
//         }
//     }
// }

void build_graph(graph *g, vector<vector<mol_info> > *points, int min_group_size){
    MolMap mol_map;

    // cout << (*points).size() << ' ' << (*points)[0].size() << ' ' << min_group_size << endl;
    int level_size = (*points)[0].size() - 2*(min_group_size - 1)-1;
    // cout << level_size << endl;
    // cout << (*points)[0].size() << endl;
    // int level_size = min_group_size;
    // cout << level_size << endl;
    g->level.resize(level_size);
    // g->level.resize(level_size-1);

    // add_vertices_edges(g, points, level_size);
    // add_vertices_edges_hashed(g, points);
    add_vertices_edges_hashed(g, points, min_group_size);
    // add_vertices(g, points, &mol_map, level_size);
    // add_edges(g, &mol_map, level_size);
}
