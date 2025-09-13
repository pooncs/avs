#pragma once
#include "avs/graph/node.hpp"

namespace avs { namespace nodes {

class ConstantNode : public avs::Node {
public:
    std::string type() const override { return "avs.nodes.Constant"; }
    Result process() override {
        outputs_["value"] = getParam("value", 0.0);
        return Result::Ok();
    }
};

}} // namespace avs::nodes
