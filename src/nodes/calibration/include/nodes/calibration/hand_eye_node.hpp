
#pragma once
#include <vector>
#include <string>
#include "calibration/hand_eye.hpp"

namespace avs::nodes {

// Minimal node interface placeholder to keep compilation decoupled.
// Integrate with your graph::Node by adapting this class accordingly.
class HandEyeCalibrationNode /* : public graph::Node */ {
public:
    HandEyeCalibrationNode() = default;

    // Inputs (set by the graph in real integration)
    std::vector<avs::calibration::PoseRT> ee2base;
    std::vector<avs::calibration::PoseRT> target2cam;
    avs::calibration::HandEyeMethod method{avs::calibration::HandEyeMethod::DANIILIDIS};

    // Outputs
    avs::calibration::PoseRT cam2ee{};

    bool process(std::string& err) noexcept;
};

} // namespace avs::nodes
