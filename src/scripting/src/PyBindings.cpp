#include "avs/scripting/PyBindings.h"
#if defined(WITH_PYTHON)
#include <pybind11/embed.h>
#include <iostream>
#include <mutex>

// Try to use AVS logging if present, otherwise fallback
#if __has_include(<avs/core/Log.h>)
  #include <avs/core/Log.h>
  #define AVS_HAS_LOG 1
#else
  #define AVS_HAS_LOG 0
#endif

namespace avs { namespace scripting {
namespace py = pybind11;

// Built-in embedded module "avs"
PYBIND11_EMBEDDED_MODULE(avs, m) {
    m.doc() = "AutoVisionStudio (AVS) Python bindings (embedded)";

    // Simple logging bridge
    m.def("log", [](const std::string& level, const std::string& msg){
#if AVS_HAS_LOG
        avs::log(level.c_str(), msg.c_str());
#else
        std::cout << "[AVS:" << level << "] " << msg << std::endl;
#endif
    }, pybind11::arg("level"), pybind11::arg("msg"));

    // Version constant for quick sanity checks from Python
    m.attr("__version__") = "0.1.0";
}

void ensure_avs_module_loaded() {
    static std::once_flag once;
    std::call_once(once, [](){
        // Touch the module to trigger registration
        py::module_::import("avs");
    });
}

}} // namespace avs::scripting
#endif // WITH_PYTHON
