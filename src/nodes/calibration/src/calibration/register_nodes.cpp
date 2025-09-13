
// Registration shim. If AVS_REGISTER_NODE is available from your graph module,
// this will register the node; otherwise it compiles to no-op.

#include "nodes/calibration/hand_eye_node.hpp"

#ifndef AVS_REGISTER_NODE
#define AVS_REGISTER_NODE(NodeType, Category, DisplayName)
#endif

AVS_REGISTER_NODE(avs::nodes::HandEyeCalibrationNode, "Calibration", "Hand-Eye Calibration");
