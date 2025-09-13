#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

// Guard the whole module behind WITH_PYTHON as required by repo options
#if defined(WITH_PYTHON)

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace avs { namespace scripting {

namespace py = pybind11;

/** RAII GIL lock helper */
class GilLock {
public:
    GilLock() : state_(py::gil_scoped_acquire()) {}
    ~GilLock() = default;
private:
    py::gil_scoped_acquire state_;
};

/**
 * PyEngine: Minimal CPython embedding controller (RAII).
 *  - Owns interpreter lifetime
 *  - Provides helpers to run strings/files and call entry points
 */
class PyEngine {
public:
    struct Options {
        std::vector<std::string> sys_paths;   // Extra sys.path entries
        bool import_avs_module = true;        // Enable built-in "avs" module bindings
    };

    PyEngine();
    ~PyEngine();

    // non-copyable
    PyEngine(const PyEngine&) = delete;
    PyEngine& operator=(const PyEngine&) = delete;

    // movable
    PyEngine(PyEngine&&) noexcept;
    PyEngine& operator=(PyEngine&&) noexcept;

    /** Initialize the interpreter if not already initialized. Safe to call multiple times. */
    void start(const Options& opts = {});

    /** Finalize interpreter if we started it. */
    void stop();

    /** True if interpreter is initialized. */
    bool isRunning() const noexcept;

    /** Append entries to sys.path (requires running interpreter). */
    void addSysPaths(const std::vector<std::string>& paths);

    /** Execute a Python string in a new ephemeral dict. */
    void runString(const std::string& code);

    /** Execute a Python file. */
    void runFile(const std::string& file_path);

    /** Import a Python module by name. */
    py::object importModule(const std::string& module_name);

    /**
     * Call a function in a Python source file.
     * The function is looked up after executing the file as a module.
     */
    py::object callFunctionInFile(const std::string& file_path,
                                  const std::string& func_name,
                                  const py::args& args = {},
                                  const py::kwargs& kwargs = {});

    /** Convenience: call process(inputs: dict, params: dict) -> dict */
    std::unordered_map<std::string, py::object>
    callProcess(const std::string& file_path,
                const std::unordered_map<std::string, py::object>& inputs,
                const std::unordered_map<std::string, py::object>& params,
                const std::string& func_name = "process");

private:
    struct Impl;
    std::unique_ptr<Impl> impl_; // pImpl per guidelines
};

}} // namespace avs::scripting

#endif // WITH_PYTHON
