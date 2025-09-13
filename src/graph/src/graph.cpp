#include "avs/graph/graph.hpp"
#include "avs/core/log.hpp"

namespace avs {

Graph::NodeId Graph::addNode(std::unique_ptr<Node> n) {
    nodes_.push_back(std::move(n));
    return nodes_.size() - 1;
}

void Graph::connect(NodeId src, const std::string& outPort, NodeId dst, const std::string& inPort) {
    edges_.push_back(Edge{src, outPort, dst, inPort});
}

Result Graph::runOnce() {
    // Very naive: process nodes in insertion order, then deliver edges from that src
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        auto& n = nodes_[i];
        auto r = n->process();
        if (!r) { return r; }
        // propagate this node's outputs to connected inputs
        for (const auto& e : edges_) {
            if (e.src == i) {
                double v = n->getOutput(e.srcPort, 0.0);
                nodes_[e.dst]->setInput(e.dstPort, v);
            }
        }
    }
    return Result::Ok();
}

} // namespace avs
