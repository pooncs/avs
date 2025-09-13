#include "avs/runtime/rest/RestServer.h"
#include <utility>

namespace avs { namespace runtime {

// Forward decl factory symbols
std::unique_ptr<IControlServer> MakeRestServer(const ControlEndpoints& ep, std::string& err);

namespace rest {

struct RestServer::Impl {
    ControlEndpoints ep{};
    bool running = false;
    // Placeholder. Real impl guarded by AVS_WITH_REST.
};

RestServer::RestServer(const ControlEndpoints& ep) : impl_(new Impl{ep, false}) {}
RestServer::~RestServer() { stop(); }

bool RestServer::start(HeadlessExecutor& /*exec*/, std::string& err) {
#ifdef AVS_WITH_REST
    (void)err; // TODO: init HTTP listener and routes
    impl_->running = true;
    return true;
#else
    err = "REST disabled at build (AVS_WITH_REST not set)";
    return false;
#endif
}

void RestServer::stop() {
#ifdef AVS_WITH_REST
    impl_->running = false;
#endif
}

}} // namespace avs::runtime::rest

using avs::runtime::IControlServer;
using avs::runtime::ControlEndpoints;

namespace avs { namespace runtime {
std::unique_ptr<IControlServer> MakeRestServer(const ControlEndpoints& ep, std::string& err) {
#ifdef AVS_WITH_REST
    (void)err;
    return std::make_unique<rest::RestServer>(ep);
#else
    (void)ep; err = "Built without REST"; return nullptr;
#endif
}
}} // namespace avs::runtime
