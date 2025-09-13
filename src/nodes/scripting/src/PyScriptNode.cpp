#include "avs/nodes/PyScriptNode.h"
#if defined(WITH_PYTHON)

#if AVS_HAS_GRAPH_API
#include <sstream>

namespace avs { namespace nodes {

PyScriptNode::PyScriptNode() {
    // Ensure interpreter ready (sys.path will be managed by app)
    engine_.start({/*sys_paths*/{}, /*import_avs_module*/true});
}

avs::Result PyScriptNode::process() {
    using py = pybind11;

    if (script_path_.empty())
        return avs::Result{false, "PyScriptNode: script_path is empty"};

    // Collect inputs from dynamic ports as a dictionary of Python objects.
    std::unordered_map<std::string, py::object> inputs;
    for (const auto& port : inputPorts()) {
        const std::string& name = port.name();
        auto v = getInput(name); // Expect variant/json-like type convertible to py::object
        inputs[name] = toPythonObject(v); // helper provided by Node base or adapter
    }

    // Parameters map: we only pass node parameters we know.
    std::unordered_map<std::string, py::object> params;
    params["entry"] = py::str(entry_);

    // Call Python
    auto outputs = engine_.callProcess(script_path_, inputs, params, entry_);

    // Write outputs back to ports
    for (const auto& kv : outputs) {
        const std::string& name = kv.first;
        if (hasOutputPort(name)) {
            setOutput(name, fromPythonObject(kv.second)); // convert back to native type
        }
    }

    return avs::Result{true, "ok"};
}

// Register into graph registry
AVS_REGISTER_NODE(avs::nodes::PyScriptNode, "Scripting/PyScript");

}} // namespace avs::nodes

#else
// Graph API not available: no-op
#endif // AVS_HAS_GRAPH_API

#endif // WITH_PYTHON
