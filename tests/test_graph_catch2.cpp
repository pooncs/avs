#include <catch2/catch_test_macros.hpp>
#include "avs/graph/graph.hpp"
#include "avs/graph/registry.hpp"

TEST_CASE("Graph basic wiring", "[graph]") {
    using namespace avs;
    auto& R = NodeRegistry::instance();
    REQUIRE(R.has("avs.nodes.Constant"));
    REQUIRE(R.has("avs.nodes.Add"));

    Graph g;
    auto n1 = R.create("avs.nodes.Constant");
    auto n2 = R.create("avs.nodes.Constant");
    auto n3 = R.create("avs.nodes.Add");

    n1->setParam("value", 10.0);
    n2->setParam("value", 5.5);

    auto id1 = g.addNode(std::move(n1));
    auto id2 = g.addNode(std::move(n2));
    auto id3 = g.addNode(std::move(n3));

    g.connect(id1, "value", id3, "a");
    g.connect(id2, "value", id3, "b");

    auto r = g.runOnce();
    REQUIRE(r);
    CHECK(g.node(id3)->getOutput("sum") == Approx(15.5));
}
