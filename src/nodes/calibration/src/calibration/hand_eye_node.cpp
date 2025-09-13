
#include "nodes/calibration/hand_eye_node.hpp"

namespace avs::nodes {

bool HandEyeCalibrationNode::process(std::string& err) noexcept {
    avs::calibration::HandEyeCalibrator calib;
    auto res = calib.compute(ee2base, target2cam, method, cam2ee);
    if (!res.ok) { err = res.msg; return false; }
    return true;
}

} // namespace avs::nodes
