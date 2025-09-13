
#pragma once
#include <vector>
#include <string>
#include "calibration/types.hpp"

namespace avs::calibration {

class HandEyeCalibrator {
public:
    // Compute camera->gripper (end-effector) from paired motions.
    // R_ee2base[i], t_ee2base[i], R_target2cam[i], t_target2cam[i]
    Result compute(const std::vector<PoseRT>& ee2base,
                   const std::vector<PoseRT>& target2cam,
                   HandEyeMethod method,
                   PoseRT& out_cam2ee) const noexcept;
};

} // namespace avs::calibration
