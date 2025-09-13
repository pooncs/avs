#include <iostream>
#include "avs/core/log.hpp"

namespace avs {
void log_info(const std::string& msg) { std::clog << "[INFO] " << msg << '\n'; }
void log_warn(const std::string& msg) { std::clog << "[WARN] " << msg << '\n'; }
void log_error(const std::string& msg) { std::cerr << "[ERROR] " << msg << '\n'; }
} // namespace avs
