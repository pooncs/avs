#include "avs/nodes/robotics/abb_movel.hpp"
#include "avs/graph/registry.hpp"

using namespace avs;

namespace avs::nodes::robotics {
AbbMoveLNode::AbbMoveLNode(const graph::NodeContext&, Params p)
: params_(std::move(p)), client_(std::make_unique<adapters::abb::AbbClient>()) {}

core::Result AbbMoveLNode::process(){
    if(!client_->is_connected()){
        auto r = client_->connect(params_.ip);
        if(!r.ok) return r;
    }
    return client_->moveL(params_.p, params_.speed, params_.zone);
}
}

static std::unique_ptr<graph::Node> AbbMoveLFactory(const graph::NodeContext& ctx){
    return std::make_unique<nodes::robotics::AbbMoveLNode>(ctx);
}

AVS_REGISTER_NODE(AbbMoveLNode, AbbMoveLFactory)
