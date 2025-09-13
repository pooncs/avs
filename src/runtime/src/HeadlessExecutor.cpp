#include "avs/runtime/HeadlessExecutor.h"

#include <chrono>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#endif

namespace avs { namespace runtime {

struct HeadlessExecutor::Impl {
    std::atomic<bool> running{false};
    HeadlessConfig cfg{};
    std::thread worker;
    std::vector<std::unique_ptr<IControlServer>> servers;
    std::mutex mtx;

    void runLoop() {
        // Minimal scheduler placeholder.
        while (running.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            // TODO: tick graph executor once integrated.
        }
    }
};

HeadlessExecutor::HeadlessExecutor() : impl_(new Impl) {}
HeadlessExecutor::~HeadlessExecutor() { stop(); }

bool HeadlessExecutor::loadGraph(const std::string& path, std::string& err) {
    // TODO: wire to graph module loader.
    (void)path; (void)err; // silence unused
    return true;
}

bool HeadlessExecutor::start(const HeadlessConfig& cfg, std::string& err) {
    if (isRunning()) return true;
    impl_->cfg = cfg;

    // Configure endpoints
    if (cfg.allow_remote) {
        ControlEndpoints ep; ep.rest = true; ep.grpc = true; ep.rest_port = cfg.rest_port; ep.grpc_port = cfg.grpc_port;
        if (auto s = MakeRestServer(ep, err)) impl_->servers.emplace_back(std::move(s));
        if (auto s = MakeGrpcServer(ep, err)) impl_->servers.emplace_back(std::move(s));
    }

    for (auto& s : impl_->servers) {
        std::string e; if (!s->start(*this, e)) { err = e; return false; }
    }

    impl_->running.store(true, std::memory_order_relaxed);
    impl_->worker = std::thread([this]{ impl_->runLoop(); });
    return true;
}

void HeadlessExecutor::stop() {
    if (!impl_->running.exchange(false)) return;
    if (impl_->worker.joinable()) impl_->worker.join();
    for (auto& s : impl_->servers) s->stop();
    impl_->servers.clear();
}

bool HeadlessExecutor::isRunning() const { return impl_->running.load(std::memory_order_relaxed); }

}} // namespace avs::runtime
