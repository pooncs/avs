
#pragma once
#include <string>
#include <vector>
#include "calibration/types.hpp"

namespace avs::calibration {

// Serialize/deserialize pose/extrinsic to a tiny JSON.
std::string to_json(const PoseRT& p);
bool from_json(const std::string& s, PoseRT& p);

// Save to project file path.
Result save_pose(const std::string& file_path, const PoseRT& p);
Result load_pose(const std::string& file_path, PoseRT& p);

// Estimate base->camera extrinsic from 3D-2D correspondences using OpenCV PnP.
// object_pts: Nx3 in base frame, image_pts: Nx2 pixels. K: 3x3 row-major, dist: k up to 8.
Result estimate_base_to_camera_pnp(const std::vector<std::array<double,3>>& object_pts,
                                   const std::vector<std::array<double,2>>& image_pts,
                                   const std::array<double,9>& K,
                                   const std::vector<double>& dist,
                                   PoseRT& out_base2cam) noexcept;

} // namespace avs::calibration
