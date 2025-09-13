
#pragma once
#include <string>
#include <vector>
#include <map>

namespace avs { namespace templates {

// Minimal data-only graph spec. No runtime deps.
struct TemplateNodeSpec {
    std::string id;           // unique id
    std::string type;         // node type key (as in registry)
    std::map<std::string, std::string> params; // simple k/v
};

struct TemplateEdgeSpec {
    std::string src;
    std::string src_port;
    std::string dst;
    std::string dst_port;
};

struct TemplateGraphSpec {
    std::string name;
    std::vector<TemplateNodeSpec> nodes;
    std::vector<TemplateEdgeSpec> edges;

    std::string to_json() const; // simple JSON writer
};

// Built-ins.
struct TemplateOptions {
    bool with_robot = true;
    bool prefer_realsense = true;
    bool prefer_photoneo = false;
};

TemplateGraphSpec MakeBinPickingTemplate(const TemplateOptions& opts = {});
TemplateGraphSpec MakeDepalletizingTemplate(const TemplateOptions& opts = {});
TemplateGraphSpec MakeInspectionTemplate(const TemplateOptions& opts = {});

// Convenience.
std::vector<TemplateGraphSpec> BuiltinTemplates(const TemplateOptions& opts = {});

// Save helper.
bool WriteTemplateJson(const TemplateGraphSpec& g, const std::string& path);

}} // namespace avs::templates
