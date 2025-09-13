
#pragma once
#include <avs/graph/node.h>
namespace avs { namespace nodes { namespace pointcloud {
class PassThrough : public avs::graph::Node {
public:
    const char* type() const override { return "pointcloud.PassThrough"; }
    avs::core::Result process() override { return {}; }
};
}}}
