#include "avs/nodes/vision/NodeAPICompat.hpp"
#if AVS_HAS_REGISTRY
#include "avs/nodes/vision/GrayscaleNode.hpp"
#include "avs/nodes/vision/BlurNode.hpp"
#include "avs/nodes/vision/CannyNode.hpp"
#include "avs/nodes/vision/TemplateMatchNode.hpp"

// Register with AVS runtime (no-op if registry not present)
AVS_REGISTER_NODE(avs::vision::GrayscaleNode);
AVS_REGISTER_NODE(avs::vision::BlurNode);
AVS_REGISTER_NODE(avs::vision::CannyNode);
AVS_REGISTER_NODE(avs::vision::TemplateMatchNode);
#endif
