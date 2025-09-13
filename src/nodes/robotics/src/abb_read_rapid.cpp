#include "avs/nodes/robotics/abb_read_rapid.hpp"
#include "avs/graph/registry.hpp"
#include "avs/core/log.hpp"

using namespace avs;

namespace avs::nodes::robotics {

AbbReadRapidNode::AbbReadRapidNode(const graph::NodeContext&, Params p)
: params_(std::move(p)), client_(std::make_unique<adapters::abb::AbbClient>()) {}

core::Result AbbReadRapidNode::process(){
    if(!client_->is_connected()){
        auto r = client_->connect(params_.ip);
        if(!r.ok) return r;
    }
    std::string out;
    auto r = client_->read_rapid(params_.task, params_.var_name, out);
    if(!r.ok) return r;
    value_ = std::move(out);
    core::log_info("RAPID[" + params_.var_name + "]=" + value_);
    return core::Result::success();
}

}

// Factory and registration
static std::unique_ptr<graph::Node> AbbReadRapidFactory(const graph::NodeContext& ctx){
    return std::make_unique<nodes::robotics::AbbReadRapidNode>(ctx);
}

AVS_REGISTER_NODE(AbbReadRapidNode, AbbReadRapidFactory)
