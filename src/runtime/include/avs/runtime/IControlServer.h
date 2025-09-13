#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace avs { namespace runtime {

class HeadlessExecutor; // fwd

struct ControlEndpoints {
    bool rest = false;
    bool grpc = false;
    uint16_t rest_port = 8080;
    uint16_t grpc_port = 50051;
};

class IControlServer {
public:
    virtual ~IControlServer() = default;
    virtual bool start(HeadlessExecutor& exec, std::string& err) = 0;
    virtual void stop() = 0;
};

// Factories (return nullptr when the feature is not compiled in)
std::unique_ptr<IControlServer> MakeRestServer(const ControlEndpoints& ep, std::string& err);
std::unique_ptr<IControlServer> MakeGrpcServer(const ControlEndpoints& ep, std::string& err);

}} // namespace avs::runtime
