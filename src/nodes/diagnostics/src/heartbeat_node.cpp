#include "avs/nodes/diagnostics/heartbeat_node.hpp"
#include "avs/core/logging.hpp"

// Assume registry macro available from graph module:
#ifndef AVS_REGISTER_NODE
#define AVS_REGISTER_NODE(ns, cls)
// Fallback no-op if registry not present in this build chunk.
#endif

AVS_REGISTER_NODE("diagnostics", HeartbeatNode)
