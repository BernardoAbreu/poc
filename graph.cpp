#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <sstream>
#include <set>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#include <iostream>

using namespace std;


struct node{
    int quality, best_quality;
    string mol_set;
    vector<int> molecules;
    list<node*> next;
    list<int> points;
    vector<long> children;
};
 
struct graph{
    vector<list<node> > level;
    // vector<node> nodes;
};


typedef map<string, node*> MolMap;
typedef unordered_map<string, node*> HashMolMap;

typedef pair<int, int> mol_info;



string join(vector<int> v, char separator, int size){
    string key = patch::to_string(v[0]);

    for(int k = 1; k < size; k++){
        key += (separator + patch::to_string(v[k]));
    }

    return key;
}

string join(vector<int> v, char separator, int size, int remove_pos){
    string key = "";
    int k;
    if(v.size() > 1 && size){
        if(remove_pos == 0){
            key = patch::to_string(v[1]);
            k = 2;
        }
        else{
            key = patch::to_string(v[0]);
            k = 1;
        }    
    }
    

    for(; k < size + 1; k++){
        // cout << "k: " << k <<  endl;
        if(k != remove_pos){
            key += (separator + patch::to_string(v[k]));
        }
    }
    // cout << "hey: size " << size <<  " remove_pos:" << remove_pos << ' ' << key << endl;
    return key;
}


void level_traverse(graph *g){
    vector<list<node> >::iterator it;
    list<node>::iterator jt;

    int i = 0;

    for (it=g->level.begin(); it != g->level.end(); ++it){
        cout << "Level " << i++ << ':';
        for (jt=(*it).begin(); jt != (*it).end(); ++jt){
            cout << ' ' << (*jt).mol_set << '(' << (*jt).quality << ',' << (*jt).best_quality << ')';

            for (list<node*>::iterator kt=(*jt).next.begin(); kt != (*jt).next.end(); ++kt){
                cout << '>' << (**kt).mol_set;
            }
            cout << '[';
            for (list<int>::iterator kt=(*jt).points.begin(); kt != (*jt).points.end(); ++kt){
                cout << (*kt) << ',';
            }
            cout << ']';

        }
        cout << endl;
    }

}


void add_vertices_edges(graph *g, vector<vector<mol_info> > *points, int level_size){
    vector<int> mol_set(level_size);
    MolMap mol_map;
    MolMap::iterator lb;

    string key;
    int gap, current_mol;

    node *last, *np;
    node n;

    for (int i = 0; i < points->size(); i++){

        last = NULL;
        for (int level = 0; level < level_size; level++){
            current_mol = (*points)[i][level].first;
            mol_set[level] = current_mol;
            gap = (*points)[i][level].second - (*points)[i][level+1].second;

            sort(mol_set.begin(),mol_set.begin()+level+1);
            
            key = join(mol_set, ',', level+1);

            lb = mol_map.lower_bound(key);

            if(lb != mol_map.end() && !(mol_map.key_comp()(key, lb->first))){
            //     // key already exists
                (lb->second)->quality += gap;
                np = lb->second;
            }
            else{
            //     // the key does not exist in the map
                n.mol_set = key;
                n.quality = gap;
                n.best_quality = gap;
                g->level[level].push_back(n);
                np = &(g->level[level].back());
                np->molecules = vector<int>(mol_set.begin(), mol_set.begin()+level+1);

                for(int k = 0; k < 4; k++){ 
                    np->children.push_back(0);
                }

                mol_map.insert(lb, MolMap::value_type(key, np));    // Use lb as a hint to insert,
                                                                // so it can avoid another lookup
            }
            np->points.push_back(i);
            if(last != NULL){
                if(!((last->children[current_mol>>5]>>(current_mol%32))&1)){
                    last->children[current_mol>>5] |= 1<<(current_mol%32);
                    last->next.push_back(np);
                }
            }
            last = np;
        }
    }
}

void add_vertices_edges_hashed(graph *g, vector<vector<mol_info> > *points, int level_size){
    vector<int> mol_set(level_size);

    string key;
    int gap, current_mol;

    node *last, *np;
    node n;

    HashMolMap mol_map;
    pair<HashMolMap::iterator, bool> search_result;
    for (int i = 0; i < points->size(); i++){
        last = NULL;

        for (int level = 0; level < level_size; level++){
            current_mol = (*points)[i][level].first;
            mol_set[level] = current_mol;
            gap = (*points)[i][level].second - (*points)[i][level+1].second;

            sort(mol_set.begin(),mol_set.begin()+level+1);
            
            key = join(mol_set, ',', level+1);

            search_result = mol_map.insert(HashMolMap::value_type(key, NULL));

            // cout << key << ':' << search_result.second<<endl;
            if(search_result.second){
                n.mol_set = key;
                n.quality = gap;
                n.best_quality = gap;
                g->level[level].push_back(n);
                np = &(g->level[level].back());
                np->molecules = vector<int>(mol_set.begin(), mol_set.begin()+level+1);

                for(int k = 0; k < 4; k++){
                    np->children.push_back(0);
                }

                (search_result.first)->second = np;
            }
            else{
                (search_result.first)->second->quality += gap;
                (search_result.first)->second->best_quality += gap;
                np = (search_result.first)->second;
            }
            np->points.push_back(i);
            if(last != NULL ){
                // cout << last->mol_set << '>' << np->mol_set << ' ' << (current_mol>>5) << ' ' << (current_mol&31) << endl;
                if(!((last->children[current_mol>>5]>>(current_mol%32))&1)){
                    last->children[current_mol>>5] |= 1<<(current_mol & 31);
                    // cout << "Current:" << last->mol_set << '>' << np->mol_set << endl;
                    last->next.push_back(np);
                }
            }
            last = np;
        }
    }
}


void add_vertices(graph *g, vector<vector<mol_info> > *points, MolMap *mol_map, int level_size){
    vector<int> mol_set(level_size);

    MolMap::iterator lb;

    string key;
    int gap;

    node *np;
    node n;

    for (int i = 0; i < points->size(); i++){

        for (int level = 0; level < level_size; level++){
            mol_set[level] = (*points)[i][level].first;
            gap = (*points)[i][level].second - (*points)[i][level+1].second;

            sort(mol_set.begin(),mol_set.begin()+level+1);
            
            key = join(mol_set, ',', level+1);

            lb = mol_map->lower_bound(key);

            if(lb != mol_map->end() && !(mol_map->key_comp()(key, lb->first))){
                // key already exists
                (lb->second)->quality += gap;
            }
            else{
                // the key does not exist in the map

                n.mol_set = key;
                n.quality = gap;
                n.best_quality = 0;
                g->level[level].push_back(n);
                np = &(g->level[level].back());
                // add_node(g, key, level, gap, np);
                np->molecules = vector<int>(mol_set.begin(), mol_set.begin()+level+1);

                mol_map->insert(lb, MolMap::value_type(key, np));    // Use lb as a hint to insert,
                                                                // so it can avoid another lookup
            }
        }
    }
}

void add_edges(graph *g, MolMap *mol_map, int level_size){
    node *current;
    string key;
    MolMap::iterator lb;

    for (int i = 0; i < level_size; i++){
        for (list<node>::iterator it=g->level[i].begin(); it != g->level[i].end(); ++it){
            current = &(*it);

            for(int removed = 0; removed < i+1; removed++){
                key = join(current->molecules, ',', i,removed);

                lb = mol_map->find(key);

                if (lb != mol_map->end()){
                    lb->second->next.push_back(current);
                }
            }
        }
    }
}

void build_graph(graph *g, vector<vector<mol_info> > *points, int level_size){
    MolMap mol_map;

    // add_vertices_edges(g, points, level_size);
    add_vertices_edges_hashed(g, points, level_size);
    // add_vertices(g, points, &mol_map, level_size);
    // add_edges(g, &mol_map, level_size);
}


void level1(graph *g, vector<node> *sel){
    list<node>::iterator it;
    list<node*>::iterator jt;

    bool possible;
    int level_size = g->level.size();

    node *n;

    for(int i = 0; i < level_size; i++){
        for (it=g->level[i].begin(); it != g->level[i].end(); ++it){
            n = &(*it);

            for(jt=n->next.begin(); jt != n->next.end(); ++jt){
                (**jt).best_quality = max(n->best_quality, (**jt).best_quality);
            }
        }
    }


    for (int i = level_size-1; i >= 0; i--){
        for (it=g->level[i].begin(); it != g->level[i].end(); ++it){
            n = &(*it);
            possible = (n->quality >= n->best_quality);
            n->best_quality = n->quality;

            for(jt=n->next.begin(); jt != n->next.end(); ++jt){
                n->best_quality = max(n->best_quality, (**jt).best_quality);
            }

            if(possible && (n->quality == n->best_quality)){
                sel->push_back(*n);
                n->best_quality++;
            }
        }
    }
}



bool myfunction (node i,node j) { return (i.quality > j.quality); }

int main(){

    graph g;
    int max_points = 5;
    int max_mol = 4;

    g.level.resize(max_mol-1);
    vector<vector<mol_info> > points(max_points);

    points[0].push_back(make_pair(4,9));
    points[0].push_back(make_pair(3,8));
    points[0].push_back(make_pair(2,5));
    points[0].push_back(make_pair(1,2));


    points[1].push_back(make_pair(1,8));
    points[1].push_back(make_pair(2,6));
    points[1].push_back(make_pair(3,3));
    points[1].push_back(make_pair(4,2));

    points[2].push_back(make_pair(3,10));
    points[2].push_back(make_pair(2,5));
    points[2].push_back(make_pair(1,4));
    points[2].push_back(make_pair(4,3));
    
    points[3].push_back(make_pair(1,7));
    points[3].push_back(make_pair(2,6));
    points[3].push_back(make_pair(3,5));
    points[3].push_back(make_pair(3,1));
    
    points[4].push_back(make_pair(2,8));
    points[4].push_back(make_pair(4,3));
    points[4].push_back(make_pair(1,2));
    points[4].push_back(make_pair(3,1));


    // print_input(points);

    build_graph(&g, &points, max_mol-1);

    level_traverse(&g);

    vector<node> selected;

    level1(&g,&selected);

    sort(selected.begin(),selected.end(), myfunction);

    cout << endl;
    level_traverse(&g);

    cout << endl;
    for(vector<node>::iterator it=selected.begin(); it != selected.end(); ++it){
        cout << (*it).mol_set << " - ";
    }
    cout << endl;

    return 0;
}