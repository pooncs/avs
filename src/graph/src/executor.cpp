
#include "avs/graph/executor.hpp"
#include <algorithm>
#include <unordered_set>
using namespace avs::graph;

bool GraphExecutor::topo_layers(std::vector<std::vector<NodeId>>& layers, std::string& err) const {
  std::unordered_map<NodeId, int> indeg;
  for (auto& kv : g_.nodes()) indeg[kv.first] = (int)g_.pred(kv.first).size();
  std::vector<NodeId> zeros;
  for (auto& kv : indeg) if (kv.second==0) zeros.push_back(kv.first);
  if (indeg.empty()) return true;

  size_t visited = 0;
  while(!zeros.empty()){
    layers.push_back({});
    auto current = std::move(zeros);
    zeros.clear();
    for(NodeId u : current){
      layers.back().push_back(u);
      ++visited;
      for(NodeId v : g_.succ(u)){
        if(--indeg[v]==0) zeros.push_back(v);
      }
    }
  }
  if (visited != g_.size()) { err = "Cycle detected"; return false; }
  return true;
}

ExecReport GraphExecutor::run(){
  ExecReport rep{};
  std::vector<std::vector<NodeId>> layers; std::string err;
  if (!topo_layers(layers, err)) { rep.ok=false; rep.errors.push_back({0, err}); return rep; }

  util::ThreadPool pool(opt_.threads?opt_.threads:1);
  NodeContext ctx;

  for (auto& layer : layers){
    std::atomic<size_t> remaining{layer.size()};
    std::mutex em;
    std::condition_variable ecv;
    for (NodeId id : layer){
      pool.submit([&, id]{
        auto it = g_.nodes().find(id);
        if (it == g_.nodes().end()) {
          std::lock_guard<std::mutex> lk(em);
          rep.ok=false; rep.errors.push_back({id, "Node missing"});
        } else {
          auto r = it->second->run(ctx, id);
          if (!r) {
            std::lock_guard<std::mutex> lk(em);
            rep.ok=false; rep.errors.push_back({id, r.msg});
          }
        }
        if (remaining.fetch_sub(1)==1) ecv.notify_one();
      });
    }
    std::unique_lock<std::mutex> lk(em);
    ecv.wait(lk, [&]{ return remaining.load()==0; });
    if (!rep.ok && opt_.stop_on_error) break;
  }
  return rep;
}
