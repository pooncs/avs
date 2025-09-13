#include <gtest/gtest.h>
#include "avs/graph/graph.hpp"
#include "avs/graph/registry.hpp"

TEST(Graph, Sum) {
    using namespace avs;
    Graph g;
    auto c1 = NodeRegistry::instance().create("avs.nodes.Constant");
    auto c2 = NodeRegistry::instance().create("avs.nodes.Constant");
    auto add = NodeRegistry::instance().create("avs.nodes.Add");
    ASSERT_TRUE(c1 && c2 && add);
    c1->setParam("value", 1.0);
    c2->setParam("value", 2.0);
    auto i1 = g.addNode(std::move(c1));
    auto i2 = g.addNode(std::move(c2));
    auto ia = g.addNode(std::move(add));
    g.connect(i1, "value", ia, "a");
    g.connect(i2, "value", ia, "b");
    auto r = g.runOnce();
    ASSERT_TRUE((bool)r);
    EXPECT_DOUBLE_EQ(g.node(ia)->getOutput("sum"), 3.0);
}
