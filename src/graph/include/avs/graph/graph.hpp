#pragma once
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <memory>
#include <unordered_map>
#include "avs/core/result.hpp"
#include "avs/graph/node.hpp"

namespace avs {

class Graph {
public:
    using NodeId = std::size_t;

    NodeId addNode(std::unique_ptr<Node> n);

    // Connect src.out -> dst.in
    void connect(NodeId src, const std::string& outPort, NodeId dst, const std::string& inPort);

    // Simple forward execution in insertion order (acyclic assumed)
    Result runOnce();

    Node* node(NodeId id) { return nodes_[id].get(); }
    const Node* node(NodeId id) const { return nodes_[id].get(); }
    std::size_t size() const { return nodes_.size(); }

private:
    struct Edge { NodeId src; std::string srcPort; NodeId dst; std::string dstPort; };
    std::vector<std::unique_ptr<Node>> nodes_;
    std::vector<Edge> edges_;
};

} // namespace avs
