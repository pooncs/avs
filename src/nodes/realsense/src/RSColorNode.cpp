#include "avs/nodes/realsense/RSColorNode.hpp"
//#include <avs/graph/Registry.hpp> // AVS_REGISTER_NODE

using namespace avs::adapters::realsense;

namespace avs { namespace nodes { namespace realsense {

RSColorNode::RSColorNode() { dev_ = std::make_shared<RealSenseDeviceManager>(); }
RSColorNode::~RSColorNode() { stop(); }

bool RSColorNode::start(){ if(!dev_->open(serial_)) return false; grab_.reset(new RealSenseFrameGrabber(dev_)); RSConfig c=cfg_; c.enable_depth=false; return grab_->start(c); }
void RSColorNode::stop(){ if(grab_) grab_->stop(); }
std::optional<RSFrame> RSColorNode::fetch(){ if(!grab_||!grab_->running()) return std::nullopt; return grab_->getColor(); }

}}}

//AVS_REGISTER_NODE(avs::nodes::realsense::RSColorNode)
