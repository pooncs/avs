#include "avs/graph/serialization.hpp"
#include "avs/util/json.hpp"
#include <fstream>

namespace avs { namespace graph {
using avs::json::Value; using avs::json::Array; using avs::json::Object;

static std::string ParamTypeToString(ParamType t){
    switch(t){ case ParamType::Bool: return "bool"; case ParamType::Int: return "int"; case ParamType::Float: return "float"; default: return "string"; }
}
static bool ParamTypeFromString(const std::string& s, ParamType& t){
    if(s=="bool"){ t=ParamType::Bool; return true; }
    if(s=="int"){ t=ParamType::Int; return true; }
    if(s=="float"){ t=ParamType::Float; return true; }
    if(s=="string"){ t=ParamType::String; return true; }
    return false;
}

static Value ToJson(const ProjectDesc& P){
    Object root; root["avs_project"] = Object{};
    auto& pr = root["avs_project"].as_object();
    pr["version"] = (int64_t)P.version;

    Object graph; // graph object
    Array nodes; nodes.reserve(P.nodes.size());
    for(const auto& n: P.nodes){
        Object jn; jn["id"] = (int64_t)n.id; jn["type"] = n.type; jn["name"] = n.name; 
        Object pos; pos["x"] = n.x; pos["y"] = n.y; jn["pos"] = std::move(pos);
        Array pars; pars.reserve(n.params.size());
        for(const auto& p : n.params){
            Object jp; jp["key"] = p.key; jp["type"] = ParamTypeToString(p.type);
            if(p.type==ParamType::Bool) jp["value"] = std::get<bool>(p.value);
            else if(p.type==ParamType::Int) jp["value"] = (int64_t)std::get<int64_t>(p.value);
            else if(p.type==ParamType::Float) jp["value"] = std::get<double>(p.value);
            else jp["value"] = std::get<std::string>(p.value);
            pars.push_back(std::move(jp));
        }
        jn["params"] = std::move(pars);
        nodes.push_back(std::move(jn));
    }
    graph["nodes"] = std::move(nodes);

    Array edges; edges.reserve(P.edges.size());
    for(const auto& e: P.edges){
        Object je; je["from"] = Object{}; je["to"] = Object{};
        auto& jf = je["from"].as_object(); jf["node"] = (int64_t)e.fromNode; jf["port"] = e.fromPort;
        auto& jt = je["to"].as_object(); jt["node"] = (int64_t)e.toNode; jt["port"] = e.toPort;
        edges.push_back(std::move(je));
    }
    graph["edges"] = std::move(edges);

    pr["graph"] = std::move(graph);
    return Value(std::move(root));
}

static bool FromJson(const Value& v, ProjectDesc& P, std::string* err){
    if(!v.is_object()) { if(err) *err="root not object"; return false; }
    const auto& root = v.as_object();
    auto it = root.find("avs_project"); if(it==root.end()||!it->second.is_object()){ if(err) *err="missing avs_project"; return false; }
    const auto& pr = it->second.as_object();
    auto itv = pr.find("version"); if(itv==pr.end()) { if(err) *err="missing version"; return false; }
    if(itv->second.is_int()) P.version = (int)itv->second.as_int(); else if(std::holds_alternative<double>(itv->second.data)) P.version = (int)itv->second.as_double(); else { if(err) *err="bad version"; return false; }

    auto itg = pr.find("graph"); if(itg==pr.end()||!itg->second.is_object()){ if(err) *err="missing graph"; return false; }
    const auto& gr = itg->second.as_object();

    // nodes
    auto itn = gr.find("nodes"); if(itn!=gr.end()){
        if(!itn->second.is_array()){ if(err) *err="nodes not array"; return false; }
        for(const auto& nv : itn->second.as_array()){
            if(!nv.is_object()){ if(err) *err="node not object"; return false; }
            const auto& no = nv.as_object();
            NodeDesc nd; 
            auto f = no.find("id"); if(f!=no.end() && (f->second.is_int() || std::holds_alternative<double>(f->second.data))) nd.id = (uint64_t)(f->second.is_int()? f->second.as_int() : (int64_t)f->second.as_double());
            f = no.find("type"); if(f!=no.end() && f->second.is_string()) nd.type = f->second.as_string(); else { if(err) *err="node missing type"; return false; }
            f = no.find("name"); if(f!=no.end() && f->second.is_string()) nd.name = f->second.as_string();
            f = no.find("pos"); if(f!=no.end() && f->second.is_object()){
                const auto& po = f->second.as_object();
                auto fx = po.find("x"); if(fx!=po.end()) nd.x = std::holds_alternative<double>(fx->second.data)? fx->second.as_double() : (double)fx->second.as_int();
                auto fy = po.find("y"); if(fy!=po.end()) nd.y = std::holds_alternative<double>(fy->second.data)? fy->second.as_double() : (double)fy->second.as_int();
            }
            f = no.find("params"); if(f!=no.end() && f->second.is_array()){
                for(const auto& pv : f->second.as_array()){
                    if(!pv.is_object()){ if(err) *err="param not object"; return false; }
                    const auto& po = pv.as_object();
                    ParamDesc pd; 
                    auto fk = po.find("key"); if(fk!=po.end() && fk->second.is_string()) pd.key = fk->second.as_string(); else { if(err) *err="param missing key"; return false; }
                    auto ft = po.find("type"); std::string ts = ft!=po.end() && ft->second.is_string()? ft->second.as_string() : std::string("string");
                    ParamTypeFromString(ts, pd.type);
                    auto fv = po.find("value");
                    if(fv!=po.end()){
                        switch(pd.type){
                            case ParamType::Bool: pd.value = fv->second.is_bool()? fv->second.as_bool() : false; break;
                            case ParamType::Int: pd.value = fv->second.is_int()? fv->second.as_int() : (int64_t)0; break;
                            case ParamType::Float: pd.value = std::holds_alternative<double>(fv->second.data)? fv->second.as_double() : 0.0; break;
                            case ParamType::String: pd.value = fv->second.is_string()? fv->second.as_string() : std::string(); break;
                        }
                    }
                    nd.params.push_back(std::move(pd));
                }
            }
            P.nodes.push_back(std::move(nd));
        }
    }

    // edges
    auto ite = gr.find("edges"); if(ite!=gr.end()){
        if(!ite->second.is_array()){ if(err) *err="edges not array"; return false; }
        for(const auto& ev : ite->second.as_array()){
            if(!ev.is_object()){ if(err) *err="edge not object"; return false; }
            const auto& eo = ev.as_object(); EdgeDesc ed; 
            auto ff = eo.find("from"); if(ff!=eo.end() && ff->second.is_object()){
                const auto& fo = ff->second.as_object();
                auto fn = fo.find("node"); if(fn!=fo.end()) ed.fromNode = (uint64_t)(std::holds_alternative<double>(fn->second.data)? fn->second.as_double() : fn->second.as_int());
                auto fp = fo.find("port"); if(fp!=fo.end() && fp->second.is_string()) ed.fromPort = fp->second.as_string();
            }
            auto tt = eo.find("to"); if(tt!=eo.end() && tt->second.is_object()){
                const auto& to = tt->second.as_object();
                auto tn = to.find("node"); if(tn!=to.end()) ed.toNode = (uint64_t)(std::holds_alternative<double>(tn->second.data)? tn->second.as_double() : tn->second.as_int());
                auto tp = to.find("port"); if(tp!=to.end() && tp->second.is_string()) ed.toPort = tp->second.as_string();
            }
            P.edges.push_back(std::move(ed));
        }
    }

    return true;
}

bool SaveProjectJson(const std::filesystem::path& file, const ProjectDesc& proj, std::string* err){
    Value root = ToJson(proj);
    std::ofstream ofs(file, std::ios::binary); if(!ofs){ if(err) *err="open failed"; return false; }
    ofs << avs::json::dump(root, 2); return true;
}

bool LoadProjectJson(const std::filesystem::path& file, ProjectDesc& out, std::string* err){
    std::ifstream ifs(file, std::ios::binary); if(!ifs){ if(err) *err="open failed"; return false; }
    std::string s((std::istreambuf_iterator<char>(ifs)), {});
    Value v; if(!avs::json::parse(s, v, err)) return false;
    out = ProjectDesc{}; // reset
    return FromJson(v, out, err);
}

}} // namespace avs::graph
