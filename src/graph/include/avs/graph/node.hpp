#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "avs/core/result.hpp"

namespace avs {

// Minimal, test-oriented node base class.
class Node {
public:
    virtual ~Node() = default;
    virtual std::string type() const = 0;

    // Simple numeric data ports for unit testing
    void setInput(const std::string& name, double v) { inputs_[name] = v; }
    bool hasInput(const std::string& name) const { return inputs_.count(name) > 0; }
    double getInput(const std::string& name, double def = 0.0) const {
        auto it = inputs_.find(name); return it == inputs_.end() ? def : it->second;
    }

    double getOutput(const std::string& name, double def = 0.0) const {
        auto it = outputs_.find(name); return it == outputs_.end() ? def : it->second;
    }

    // Params (e.g., for ConstantNode)
    void setParam(const std::string& name, double v) { params_[name] = v; }
    double getParam(const std::string& name, double def = 0.0) const {
        auto it = params_.find(name); return it == params_.end() ? def : it->second;
    }

    virtual Result process() = 0; // compute outputs from inputs/params

protected:
    std::unordered_map<std::string, double> inputs_;
    std::unordered_map<std::string, double> outputs_;
    std::unordered_map<std::string, double> params_;
};

using NodePtr = std::unique_ptr<Node>;

} // namespace avs
