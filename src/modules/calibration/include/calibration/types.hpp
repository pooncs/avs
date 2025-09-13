
#pragma once
#include <array>
#include <string>
#include <vector>

namespace avs::calibration {

struct Result {
    bool ok{false};
    std::string msg;
    static Result success(std::string m = {}) { return {true, std::move(m)}; }
    static Result failure(std::string m) { return {false, std::move(m)}; }
};

struct PoseRT {
    std::array<double, 9> R{}; // row-major 3x3
    std::array<double, 3> t{}; // x,y,z
};

struct CalibrationOutput {
    PoseRT cam_to_ee;   // camera->end-effector
    PoseRT base_to_cam; // base->camera (optional)
};

enum class HandEyeMethod {
    TSAI,
    PARK,
    DANIILIDIS
};

} // namespace avs::calibration
