
#pragma once
#include <cstdint>
#include <string>

namespace avs::graph {
using NodeId = uint64_t;
struct NodeInfo {
  NodeId id{};
  std::string type;
  std::string name;
};
}
