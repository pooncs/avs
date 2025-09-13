#include "avs/scripting/PyEngine.h"
#if defined(WITH_PYTHON)
#include "avs/scripting/PyBindings.h"
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace avs { namespace scripting {

struct PyEngine::Impl {
    bool owns_interpreter = false;
    std::unique_ptr<py::scoped_interpreter> interp; // RAII interpreter
};

PyEngine::PyEngine() : impl_(std::make_unique<Impl>()) {}
PyEngine::~PyEngine() { stop(); }

PyEngine::PyEngine(PyEngine&& other) noexcept : impl_(std::move(other.impl_)) {}
PyEngine& PyEngine::operator=(PyEngine&& other) noexcept {
    if (this != &other) impl_ = std::move(other.impl_);
    return *this;
}

void PyEngine::start(const Options& opts) {
    if (isRunning()) {
        // still allow adding sys.path and (re)loading avs module
        addSysPaths(opts.sys_paths);
        if (opts.import_avs_module) ensure_avs_module_loaded();
        return;
    }
    impl_->interp = std::make_unique<py::scoped_interpreter>();
    impl_->owns_interpreter = true;

    if (!opts.sys_paths.empty()) addSysPaths(opts.sys_paths);
    if (opts.import_avs_module) ensure_avs_module_loaded();
}

void PyEngine::stop() {
    if (impl_ && impl_->owns_interpreter && impl_->interp) {
        // Let scoped_interpreter dtor finalize Python.
        impl_->interp.reset();
        impl_->owns_interpreter = false;
    }
}

bool PyEngine::isRunning() const noexcept {
    try {
        return impl_ && impl_->interp != nullptr;
    } catch (...) { return false; }
}

void PyEngine::addSysPaths(const std::vector<std::string>& paths) {
    if (!isRunning() || paths.empty()) return;
    GilLock gil;
    auto sys = py::module_::import("sys");
    auto py_path = sys.attr("path");
    for (const auto& p : paths) {
        py_path.attr("append")(py::str(p));
    }
}

void PyEngine::runString(const std::string& code) {
    if (!isRunning()) throw std::runtime_error("Python interpreter not running");
    GilLock gil;
    py::exec(py::str(code));
}

void PyEngine::runFile(const std::string& file_path) {
    if (!isRunning()) throw std::runtime_error("Python interpreter not running");
    GilLock gil;
    auto builtins = py::module_::import("builtins");
    auto globals = py::dict("__builtins__"_a=builtins);
    py::eval_file(file_path, globals, globals);
}

py::object PyEngine::importModule(const std::string& module_name) {
    if (!isRunning()) throw std::runtime_error("Python interpreter not running");
    GilLock gil;
    return py::module_::import(module_name.c_str());
}

py::object PyEngine::callFunctionInFile(const std::string& file_path,
                                        const std::string& func_name,
                                        const py::args& args,
                                        const py::kwargs& kwargs) {
    if (!isRunning()) throw std::runtime_error("Python interpreter not running");
    GilLock gil;
    // Load as a module by path
    py::module_ importlib = py::module_::import("importlib.util");
    py::object spec = importlib.attr("spec_from_file_location")(py::str("_avs_script_"), py::str(file_path));
    py::object module = importlib.attr("module_from_spec")(spec);
    spec.attr("loader").attr("exec_module")(module);
    py::object func = module.attr(func_name.c_str());
    return func(*args, **kwargs);
}

std::unordered_map<std::string, py::object>
PyEngine::callProcess(const std::string& file_path,
                      const std::unordered_map<std::string, py::object>& inputs,
                      const std::unordered_map<std::string, py::object>& params,
                      const std::string& func_name) {
    if (!isRunning()) throw std::runtime_error("Python interpreter not running");
    GilLock gil;
    py::dict py_inputs; for (auto& kv : inputs) py_inputs[py::str(kv.first)] = kv.second;
    py::dict py_params; for (auto& kv : params) py_params[py::str(kv.first)] = kv.second;
    py::object out = callFunctionInFile(file_path, func_name, py::make_tuple(py_inputs, py_params));
    std::unordered_map<std::string, py::object> result;
    if (!py::isinstance<py::dict>(out)) return result;
    py::dict d = out;
    for (auto item : d) {
        std::string k = py::str(item.first);
        result.emplace(k, py::reinterpret_borrow<py::object>(item.second));
    }
    return result;
}

}} // namespace avs::scripting
#endif // WITH_PYTHON
