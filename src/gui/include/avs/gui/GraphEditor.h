#pragma once
#include <memory>

namespace avs { namespace graph { class Graph; } }

namespace avs::gui {

class IGraphEditor {
public:
    virtual ~IGraphEditor() = default;
    virtual void setGraph(std::shared_ptr<avs::graph::Graph> g) = 0;
    virtual void draw(const char* id) = 0; // draw inside a window or dockspace
};

} // namespace avs::gui
