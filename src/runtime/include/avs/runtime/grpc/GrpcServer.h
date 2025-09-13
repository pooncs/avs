#pragma once
#include <memory>
#include <string>
#include "avs/runtime/IControlServer.h"

namespace avs { namespace runtime { namespace grpcsvc {

class GrpcServer final : public IControlServer {
public:
    explicit GrpcServer(const ControlEndpoints& ep);
    ~GrpcServer() override;
    bool start(HeadlessExecutor& exec, std::string& err) override;
    void stop() override;
private:
    struct Impl; std::unique_ptr<Impl> impl_;
};

}}} // namespace avs::runtime::grpcsvc
