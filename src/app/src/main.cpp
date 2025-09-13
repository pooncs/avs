#include <iostream>
#include "avs/graph/graph.hpp"
#include "avs/graph/registry.hpp"
#include "avs/nodes/add_node.hpp"
#include "avs/nodes/constant_node.hpp"

int main() {
    using avs::Graph; using avs::NodeRegistry;
    Graph g;
    auto c1 = NodeRegistry::instance().create("avs.nodes.Constant");
    auto c2 = NodeRegistry::instance().create("avs.nodes.Constant");
    auto add = NodeRegistry::instance().create("avs.nodes.Add");
    if(!c1 || !c2 || !add) { std::cerr << "Node registry missing types\n"; return 1; }

    c1->setParam("value", 2.0);
    c2->setParam("value", 3.0);

    auto id1 = g.addNode(std::move(c1));
    auto id2 = g.addNode(std::move(c2));
    auto id3 = g.addNode(std::move(add));

    g.connect(id1, "value", id3, "a");
    g.connect(id2, "value", id3, "b");

    auto r = g.runOnce();
    if(!r) { std::cerr << r.msg << "\n"; return 2; }

    std::cout << "2 + 3 = " << g.node(id3)->getOutput("sum") << "\n";
    return 0;
}
