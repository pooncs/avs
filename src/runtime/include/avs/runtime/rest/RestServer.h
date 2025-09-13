#pragma once
#include <memory>
#include <string>
#include "avs/runtime/IControlServer.h"

namespace avs { namespace runtime { namespace rest {

class RestServer final : public IControlServer {
public:
    explicit RestServer(const ControlEndpoints& ep);
    ~RestServer() override;
    bool start(HeadlessExecutor& exec, std::string& err) override;
    void stop() override;
private:
    struct Impl; std::unique_ptr<Impl> impl_;
};

}}} // namespace avs::runtime::rest
