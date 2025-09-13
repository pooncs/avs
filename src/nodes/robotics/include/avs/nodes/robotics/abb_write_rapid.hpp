#pragma once
#include "avs/graph/node.hpp"
#include "avs/adapters/abb/abb_client.hpp"
#include <memory>

namespace avs::nodes::robotics {
class AbbWriteRapidNode : public avs::graph::Node {
public:
    struct Params { std::string ip; std::string task{"T_ROB1"}; std::string var_name; std::string value; };
    explicit AbbWriteRapidNode(const avs::graph::NodeContext&, Params p = {});
    avs::core::Result process() override;
private:
    Params params_;
    std::unique_ptr<avs::adapters::abb::IAbbClient> client_;
};
}
