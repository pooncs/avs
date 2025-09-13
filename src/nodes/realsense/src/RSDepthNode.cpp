#include "avs/nodes/realsense/RSDepthNode.hpp"
//#include <avs/graph/Registry.hpp>

using namespace avs::adapters::realsense;

namespace avs { namespace nodes { namespace realsense {

RSDepthNode::RSDepthNode() { dev_ = std::make_shared<RealSenseDeviceManager>(); }
RSDepthNode::~RSDepthNode() { stop(); }

bool RSDepthNode::start(){ if(!dev_->open(serial_)) return false; grab_.reset(new RealSenseFrameGrabber(dev_)); RSConfig c=cfg_; c.enable_color=false; return grab_->start(c); }
void RSDepthNode::stop(){ if(grab_) grab_->stop(); }
std::optional<RSFrame> RSDepthNode::fetch(){ if(!grab_||!grab_->running()) return std::nullopt; return grab_->getDepth(); }

}}}

//AVS_REGISTER_NODE(avs::nodes::realsense::RSDepthNode)
