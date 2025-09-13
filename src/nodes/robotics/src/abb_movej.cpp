#include "avs/nodes/robotics/abb_movej.hpp"
#include "avs/graph/registry.hpp"

using namespace avs;

namespace avs::nodes::robotics {
AbbMoveJNode::AbbMoveJNode(const graph::NodeContext&, Params p)
: params_(std::move(p)), client_(std::make_unique<adapters::abb::AbbClient>()) {}

core::Result AbbMoveJNode::process(){
    if(!client_->is_connected()){
        auto r = client_->connect(params_.ip);
        if(!r.ok) return r;
    }
    if(params_.use_joints) return client_->moveJ(params_.q, params_.speed, params_.zone);
    else return client_->moveJ(params_.p, params_.speed, params_.zone);
}
}

static std::unique_ptr<graph::Node> AbbMoveJFactory(const graph::NodeContext& ctx){
    return std::make_unique<nodes::robotics::AbbMoveJNode>(ctx);
}

AVS_REGISTER_NODE(AbbMoveJNode, AbbMoveJFactory)
