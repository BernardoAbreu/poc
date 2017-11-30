#include "graph.h"
#include "util.h"


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
