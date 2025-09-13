
#pragma once
#include <avs/graph/node.h>
namespace avs { namespace nodes { namespace robotics {
class MoveJ : public avs::graph::Node {
public:
    const char* type() const override { return "robotics.MoveJ"; }
    avs::core::Result process() override { return {}; }
};
}}}
