#pragma once
#include "avs/graph/node.hpp"

namespace avs { namespace nodes {

class AddNode : public avs::Node {
public:
    std::string type() const override { return "avs.nodes.Add"; }
    Result process() override {
        if (!hasInput("a") || !hasInput("b"))
            return Result::Error("AddNode missing inputs 'a' or 'b'");
        double s = getInput("a") + getInput("b");
        outputs_["sum"] = s;
        return Result::Ok();
    }
};

}} // namespace avs::nodes
