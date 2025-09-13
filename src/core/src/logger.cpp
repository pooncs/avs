
#include <avs/core/logger.h>
#include <iostream>
namespace avs { namespace core {
void log_info(const std::string& s){ std::clog << "[I] " << s << ""; }
void log_warn(const std::string& s){ std::clog << "[W] " << s << ""; }
void log_err(const std::string& s){ std::cerr << "[E] " << s << ""; }
}} // namespace
