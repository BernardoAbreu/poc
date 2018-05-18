#include "graph.h"
#include "util.h"



std::ostream& operator<<(std::ostream& out, const Graph& g){
    // std::vector<std::list<Node> >::const_iterator it;
    // std::list<Node>::const_iterator jt;

    int i = 0;

    // for (it=g.level.cbegin(); it != g.level.cend(); ++it){
    for(auto& level: g.level){
        out << "Level " << i++ << ':';
        // for (jt=(*it).cbegin(); jt != (*it).cend(); ++jt){
        for(auto& node : level){
            // out << ' ' << (*jt).pat.str << '(' << (*jt).pat.quality << ',' << (*jt).pat.best_quality << ')';
            out << ' ' << node.pat.str << '(' << node.pat.quality << ',' << node.pat.best_quality << ')';

            // for(std::list<Node*>::const_iterator kt=(*jt).next.cbegin(); kt != (*jt).next.cend(); ++kt){
            for(auto& child: node.next){
                out << '>' << child->pat.str;
            }
            out << '[' << join(node.pat.points, ',') << ']';

        }
        out << std::endl;
    }
    return out;
}
