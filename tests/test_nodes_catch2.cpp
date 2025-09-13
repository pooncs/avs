#include <catch2/catch_test_macros.hpp>
#include "avs/graph/registry.hpp"

TEST_CASE("Node registry create", "[nodes]") {
    using namespace avs;
    auto& R = NodeRegistry::instance();
    auto c = R.create("avs.nodes.Constant");
    auto a = R.create("avs.nodes.Add");
    REQUIRE(c);
    REQUIRE(a);
}
