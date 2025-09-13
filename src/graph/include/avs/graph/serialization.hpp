#pragma once
#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include <filesystem>

namespace avs { namespace graph {

// Parameter value types
using ParamValue = std::variant<bool,int64_t,double,std::string>;

enum class ParamType { Bool, Int, Float, String };

struct ParamDesc {
    std::string key;
    ParamType type{ParamType::String};
    ParamValue value{}; // must match 'type'
};

struct NodeDesc {
    uint64_t id{0};
    std::string type;   // registry key
    std::string name;   // display name
    double x{0}, y{0};  // UI pos
    std::vector<ParamDesc> params;
};

struct EdgeDesc {
    uint64_t fromNode{0};
    std::string fromPort; // output port name
    uint64_t toNode{0};
    std::string toPort;   // input port name
};

struct ProjectDesc {
    int version{1};
    std::vector<NodeDesc> nodes;
    std::vector<EdgeDesc> edges;
};

// Serialize/deserialize ProjectDesc to JSON file.
// Returns true on success. On failure, returns false and sets err if provided.
bool SaveProjectJson(const std::filesystem::path& file, const ProjectDesc& proj, std::string* err = nullptr);
bool LoadProjectJson(const std::filesystem::path& file, ProjectDesc& out, std::string* err = nullptr);

}} // namespace avs::graph
