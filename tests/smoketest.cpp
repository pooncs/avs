
#include <cassert>
#include <avs/graph/graph.h>
#include <avs/graph/node.h>
int main(){
    avs::graph::Graph g; 
    assert(g.node_count()==0);
    return 0;
}
