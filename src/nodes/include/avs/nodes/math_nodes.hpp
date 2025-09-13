
#pragma once
#include "avs/graph/node.hpp"
#include "avs/graph/registry.hpp"
#include <atomic>

namespace avs::nodes {
// Demo: counter node keeps state across runs.
class CounterNode : public avs::graph::INode {
public:
  const char* type() const override { return "Counter"; }
  avs::core::Result run(avs::graph::NodeContext& ctx, avs::graph::NodeId self) override {
    auto& c = ctx.state<std::size_t>(self, 0);
    ++c;
    return avs::core::Result::success();
  }
};

class AddNode : public avs::graph::INode {
  int a_, b_;
public:
  AddNode(): a_(1), b_(2) {}
  const char* type() const override { return "Add"; }
  avs::core::Result run(avs::graph::NodeContext& ctx, avs::graph::NodeId self) override {
    (void)ctx; (void)self; // Not used here.
    volatile int s = a_ + b_;
    (void)s;
    return avs::core::Result::success();
  }
};
} // namespace avs::nodes

AVS_REGISTER_NODE("Counter", avs::nodes::CounterNode)
AVS_REGISTER_NODE("Add", avs::nodes::AddNode)
