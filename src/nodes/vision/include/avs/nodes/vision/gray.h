
#pragma once
#include <avs/graph/node.h>
namespace avs { namespace nodes { namespace vision {
class Gray : public avs::graph::Node {
public:
    const char* type() const override { return "vision.Gray"; }
    avs::core::Result process() override { return {}; }
};
}}}
