
#pragma once
#include <string>
#include <unordered_map>
namespace avs { namespace util {
// Tiny placeholder for JSON-like config (replace with rapidjson/nlohmann later)
using kv = std::unordered_map<std::string, std::string>;
}} // namespace
