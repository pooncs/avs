#pragma once

#if defined(WITH_PYTHON)
#include <pybind11/pybind11.h>

namespace avs { namespace scripting {

// Initialize extra runtime glue if needed
void ensure_avs_module_loaded();

}} // namespace avs::scripting
#endif // WITH_PYTHON
