#include "avs/graph/registry.hpp"
#include <stdexcept>

namespace avs {

NodeRegistry& NodeRegistry::instance() {
    static NodeRegistry inst; return inst;
}

void NodeRegistry::registerNode(const std::string& type, NodeFactory f) {
    std::lock_guard<std::mutex> lock(mtx_);
    map_[type] = std::move(f);
}

bool NodeRegistry::has(const std::string& type) const {
    std::lock_guard<std::mutex> lock(mtx_);
    return map_.count(type) > 0;
}

std::unique_ptr<Node> NodeRegistry::create(const std::string& type) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = map_.find(type);
    if (it == map_.end()) return nullptr;
    return it->second();
}

} // namespace avs
