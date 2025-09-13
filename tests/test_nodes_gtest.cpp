#include <gtest/gtest.h>
#include "avs/graph/registry.hpp"

TEST(Nodes, Registry) {
    using namespace avs;
    auto& R = NodeRegistry::instance();
    EXPECT_TRUE(R.has("avs.nodes.Constant"));
    EXPECT_TRUE(R.has("avs.nodes.Add"));
}
