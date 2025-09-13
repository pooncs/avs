
#pragma once
#include <vector>
#include <queue>
#include <atomic>
#include <unordered_map>
#include <functional>
#include <optional>
#include "avs/core/result.hpp"
#include "avs/core/log.hpp"
#include "avs/util/thread_pool.hpp"
#include "avs/graph/graph.hpp"

namespace avs::graph {
struct ExecOptions {
  unsigned threads = std::thread::hardware_concurrency();
  bool stop_on_error = true;
};

struct ExecReport {
  bool ok{true};
  std::vector<std::pair<NodeId,std::string>> errors; // id,msg
};

class GraphExecutor {
  const Graph& g_;
  ExecOptions opt_;
public:
  explicit GraphExecutor(const Graph& g, ExecOptions opt = {}) : g_(g), opt_(opt) {}
  ExecReport run();
private:
  bool topo_layers(std::vector<std::vector<NodeId>>& layers, std::string& err) const;
};
} // namespace avs::graph
