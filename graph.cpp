// #include <algorithm>
// #include <sstream>
// #include <set>
// #include <math.h>
// #include <iostream>

#include "graph.h"
#include "util.h"

// using namespace std;

void Graph::level_traverse(std::ostream& out){
    std::vector<std::list<node> >::iterator it;
    std::list<node>::iterator jt;

    int i = 0;

    for (it=this->level.begin(); it != this->level.end(); ++it){
        out << "Level " << i++ << ':';
        for (jt=(*it).begin(); jt != (*it).end(); ++jt){
            out << ' ' << (*jt).pat.mol_set << '(' << (*jt).pat.quality << ',' << (*jt).pat.best_quality << ')';

            for (std::list<node*>::iterator kt=(*jt).next.begin(); kt != (*jt).next.end(); ++kt){
                out << '>' << (**kt).pat.mol_set;
            }
            out << '[' << join((*jt).pat.points, ',') << ']';

        }
        out << std::endl;
    }
}

std::ostream& operator<<(std::ostream& out, const Graph& g){
    std::vector<std::list<node> >::const_iterator it;
    std::list<node>::const_iterator jt;

    int i = 0;

    for (it=g.level.cbegin(); it != g.level.cend(); ++it){
        out << "Level " << i++ << ':';
        for (jt=(*it).cbegin(); jt != (*it).cend(); ++jt){
            out << ' ' << (*jt).pat.mol_set << '(' << (*jt).pat.quality << ',' << (*jt).pat.best_quality << ')';

            for (std::list<node*>::const_iterator kt=(*jt).next.cbegin(); kt != (*jt).next.cend(); ++kt){
                out << '>' << (**kt).pat.mol_set;
            }
            out << '[' << join((*jt).pat.points, ',') << ']';

        }
        out << std::endl;
    }
    return out;
}

// void add_vertices_edges(graph *g, std::vector<std::vector<mol_info> > *points){
//     int level_size = g->level.size();
//     std::vector<int> mol_set(level_size);
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
//                 np->molecules = std::vector<int>(mol_set.begin(), mol_set.begin()+level+1);

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

// void add_vertices_edges_hashed(graph *g, std::vector<std::vector<mol_info> > *points){
//     int level_size = g->level.size();
//     std::vector<int> mol_set(level_size);

//     string key;
//     int gap, current_mol, points_size;

//     node *last, *np;
//     node n;

//     HashMolMap mol_map;

//     points_size = points->size();
//     pair<HashMolMap::iterator, bool> search_result;
//     for (int i = 0; i < points_size; i++){
//         last = NULL;
//         // std::vector<int> mol_set;
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

//             // std::cout << key << ':' << search_result.second<<std::endl;
//             if(search_result.second){
//                 n.mol_set = key;
//                 n.quality = gap;
//                 n.best_quality = gap;
//                 g->level[level].push_back(n);
//                 np = &(g->level[level].back());
//                 np->molecules = std::vector<int>(mol_set.begin(), mol_set.begin()+level+1);

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
//                 // std::cout << last->mol_set << '>' << np->mol_set << ' ' << (current_mol>>5) << ' ' << (current_mol&31) << std::endl;
//                 if(!((last->children[current_mol>>5]>>(current_mol%32))&1)){
//                     last->children[current_mol>>5] |= 1<<(current_mol & 31);
//                     // std::cout << "Current:" << last->mol_set << '>' << np->mol_set << std::endl;
//                     last->next.push_back(np);
//                 }
//             }
//             last = np;
//         }
//     }
// }


// void add_vertices(graph *g, std::vector<std::vector<mol_info> > *points, MolMap *mol_map){
//     int level_size = g->level.size();
//     std::vector<int> mol_set(level_size);

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
//                 np->pat.molecules = std::vector<int>(mol_set.begin(), mol_set.begin()+level+1);

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
//         for (std::list<node>::iterator it=g->level[i].begin(); it != g->level[i].end(); ++it){
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