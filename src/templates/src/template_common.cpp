
#include "avs/templates/template_common.hpp"
#include <sstream>
#include <fstream>

using namespace avs::templates;

static std::string escape(const std::string& s){
    std::string o; o.reserve(s.size()+8);
    for(char c: s){
        switch(c){
            case '\': o += "\\"; break;
            case '"':  o += "\""; break;
            case '
': o += "\n"; break;
            case '': o += "\r"; break;
            case '	': o += "\t"; break;
            default: o += c; break;
        }
    }
    return o;
}

std::string TemplateGraphSpec::to_json() const {
    std::ostringstream os;
    os << "{
";
    os << "  "name": "" << escape(name) << "",
";
    os << "  "nodes": [
";
    for(size_t i=0;i<nodes.size();++i){
        const auto& n = nodes[i];
        os << "    {
";
        os << "      "id": "" << escape(n.id) << "",
";
        os << "      "type": "" << escape(n.type) << "",
";
        os << "      "params": {";
        bool first=true;
        for(const auto& kv: n.params){
            if(!first) os << ", "; first=false;
            os << """ << escape(kv.first) << "": "" << escape(kv.second) << """;
        }
        os << "}
";
        os << "    }" << (i+1<nodes.size()?",":"") << "
";
    }
    os << "  ],
";
    os << "  "edges": [
";
    for(size_t i=0;i<edges.size();++i){
        const auto& e = edges[i];
        os << "    {
";
        os << "      "src": "" << escape(e.src) << "",
";
        os << "      "src_port": "" << escape(e.src_port) << "",
";
        os << "      "dst": "" << escape(e.dst) << "",
";
        os << "      "dst_port": "" << escape(e.dst_port) << ""
";
        os << "    }" << (i+1<edges.size()?",":"") << "
";
    }
    os << "  ]
";
    os << "}
";
    return os.str();
}

bool avs::templates::WriteTemplateJson(const TemplateGraphSpec& g, const std::string& path){
    std::ofstream f(path, std::ios::binary);
    if(!f) return false;
    auto s = g.to_json();
    f.write(s.data(), (std::streamsize)s.size());
    return (bool)f;
}

std::vector<TemplateGraphSpec> avs::templates::BuiltinTemplates(const TemplateOptions& opts){
    return { MakeBinPickingTemplate(opts), MakeDepalletizingTemplate(opts), MakeInspectionTemplate(opts) };
}
