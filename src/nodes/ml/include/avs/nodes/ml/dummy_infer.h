
#pragma once
#include <avs/graph/node.h>
namespace avs { namespace nodes { namespace ml {
class DummyInfer : public avs::graph::Node {
public:
    const char* type() const override { return "ml.DummyInfer"; }
    avs::core::Result process() override { return {}; }
};
}}}
