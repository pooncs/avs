#pragma once
#include "avs/graph/node.hpp"
#include "avs/adapters/abb/abb_client.hpp"
#include <memory>

namespace avs::nodes::robotics {
class AbbReadRapidNode : public avs::graph::Node {
public:
    struct Params { std::string ip; std::string task{"T_ROB1"}; std::string var_name; };
    explicit AbbReadRapidNode(const avs::graph::NodeContext&, Params p = {});
    avs::core::Result process() override;
    const std::string& value() const { return value_; }
private:
    Params params_;
    std::unique_ptr<avs::adapters::abb::IAbbClient> client_;
    std::string value_;
};
}
