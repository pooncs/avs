#pragma once
#include "avs/graph/node.hpp"
#include "avs/adapters/abb/abb_client.hpp"
#include <memory>

namespace avs::nodes::robotics {
class AbbMoveJNode : public avs::graph::Node {
public:
    struct Params { std::string ip; bool use_joints{true}; adapters::abb::JointPositions q; adapters::abb::Pose p; double speed{50.0}; double zone{5.0}; };
    explicit AbbMoveJNode(const avs::graph::NodeContext&, Params p = {});
    avs::core::Result process() override;
private:
    Params params_;
    std::unique_ptr<avs::adapters::abb::IAbbClient> client_;
};
}
