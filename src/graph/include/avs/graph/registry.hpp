#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "avs/graph/node.hpp"

namespace avs {

using NodeFactory = std::function<std::unique_ptr<Node>()>;

class NodeRegistry {
public:
    static NodeRegistry& instance();

    void registerNode(const std::string& type, NodeFactory f);
    bool has(const std::string& type) const;
    std::unique_ptr<Node> create(const std::string& type) const;

private:
    NodeRegistry() = default;
    NodeRegistry(const NodeRegistry&) = delete;
    NodeRegistry& operator=(const NodeRegistry&) = delete;

    mutable std::mutex mtx_;
    std::unordered_map<std::string, NodeFactory> map_;
};

// Helper macro to register a node type with a key
#define AVS_REGISTER_NODE(NodeType, Key) \
    namespace { \
    struct NodeType##Registrar { \
        NodeType##Registrar() { \
            avs::NodeRegistry::instance().registerNode(Key, [](){ return std::make_unique<NodeType>(); }); \
        } \
    }; \
    static NodeType##Registrar global_##NodeType##_registrar; \
    }

} // namespace avs
