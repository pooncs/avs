#include "avs/nodes/robotics/abb_write_rapid.hpp"
#include "avs/graph/registry.hpp"

using namespace avs;

namespace avs::nodes::robotics {
AbbWriteRapidNode::AbbWriteRapidNode(const graph::NodeContext&, Params p)
: params_(std::move(p)), client_(std::make_unique<adapters::abb::AbbClient>()) {}

core::Result AbbWriteRapidNode::process(){
    if(!client_->is_connected()){
        auto r = client_->connect(params_.ip);
        if(!r.ok) return r;
    }
    return client_->write_rapid(params_.task, params_.var_name, params_.value);
}
}

static std::unique_ptr<graph::Node> AbbWriteRapidFactory(const graph::NodeContext& ctx){
    return std::make_unique<nodes::robotics::AbbWriteRapidNode>(ctx);
}

AVS_REGISTER_NODE(AbbWriteRapidNode, AbbWriteRapidFactory)
