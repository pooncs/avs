#include "avs/runtime/grpc/GrpcServer.h"
#include <utility>

namespace avs { namespace runtime {

// Forward decl factory symbols
std::unique_ptr<IControlServer> MakeGrpcServer(const ControlEndpoints& ep, std::string& err);

namespace grpcsvc {

struct GrpcServer::Impl {
    ControlEndpoints ep{};
    bool running = false;
    // Placeholder. Real impl guarded by AVS_WITH_GRPC.
};

GrpcServer::GrpcServer(const ControlEndpoints& ep) : impl_(new Impl{ep, false}) {}
GrpcServer::~GrpcServer() { stop(); }

bool GrpcServer::start(HeadlessExecutor& /*exec*/, std::string& err) {
#ifdef AVS_WITH_GRPC
    (void)err; // TODO: init gRPC server and services
    impl_->running = true;
    return true;
#else
    err = "gRPC disabled at build (AVS_WITH_GRPC not set)";
    return false;
#endif
}

void GrpcServer::stop() {
#ifdef AVS_WITH_GRPC
    impl_->running = false;
#endif
}

}} // namespace avs::runtime::grpcsvc

using avs::runtime::IControlServer;
using avs::runtime::ControlEndpoints;

namespace avs { namespace runtime {
std::unique_ptr<IControlServer> MakeGrpcServer(const ControlEndpoints& ep, std::string& err) {
#ifdef AVS_WITH_GRPC
    (void)err;
    return std::make_unique<grpcsvc::GrpcServer>(ep);
#else
    (void)ep; err = "Built without gRPC"; return nullptr;
#endif
}
}} // namespace avs::runtime
