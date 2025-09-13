#pragma once
#include <string>
#include <vector>

namespace avs { namespace fs {
inline bool exists(const std::string&) { return true; }
inline std::vector<std::string> listdir(const std::string&) { return {}; }
}} // namespace avs::fs
