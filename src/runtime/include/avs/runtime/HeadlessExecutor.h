#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "avs/runtime/IControlServer.h"

namespace avs { namespace runtime {

struct HeadlessConfig {
    int max_workers = 0;            // 0 => hw_concurrency
    bool allow_remote = false;      // enable REST/gRPC if compiled
    uint16_t rest_port = 8080;
    uint16_t grpc_port = 50051;
    bool run_as_service = false;    // Windows only
    std::string graph_path;         // optional
};

class HeadlessExecutor {
public:
    HeadlessExecutor();
    ~HeadlessExecutor();

    bool loadGraph(const std::string& path, std::string& err);
    bool start(const HeadlessConfig& cfg, std::string& err);
    void stop();
    bool isRunning() const;

private:
    struct Impl; // pImpl
    std::unique_ptr<Impl> impl_;
};

}} // namespace avs::runtime
