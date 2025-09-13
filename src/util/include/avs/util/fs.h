
#pragma once
#include <string>
#include <fstream>
#include <sstream>
namespace avs { namespace util {
inline std::string read_text(const std::string& p){ std::ifstream f(p); std::stringstream ss; ss<<f.rdbuf(); return ss.str(); }
inline void write_text(const std::string& p, const std::string& s){ std::ofstream f(p); f<<s; }
}} // namespace
