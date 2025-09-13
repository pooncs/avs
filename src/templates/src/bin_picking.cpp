
#include "avs/templates/bin_picking.hpp"

using namespace avs::templates;

TemplateGraphSpec avs::templates::MakeBinPickingTemplate(const TemplateOptions& opts){
    TemplateGraphSpec g; g.name = "bin_picking";

    const bool use_rs = opts.prefer_realsense && !opts.prefer_photoneo;

    // Nodes
    g.nodes.push_back({"cam", use_rs ? "camera.realsense.Capture" : "camera.photoneo.Capture", {}});
    g.nodes.push_back({"depth", "vision.DepthToCloud", {}});
    g.nodes.push_back({"seg", "ml.Detector", {{"model", "bin_objects.onnx"}}});
    g.nodes.push_back({"pose", "geom.PoseEstimator", {{"method", "icp"}}});
    g.nodes.push_back({"grasp", "plan.GraspPlanner", {{"strategy", "suction"}}});
    if(opts.with_robot){
        g.nodes.push_back({"move", "robot.abb.MoveToPose", {{"speed", "100"}}});
    }

    // Edges
    g.edges.push_back({"cam","depth","depth","depth"}); // depth image out -> depth input
    g.edges.push_back({"cam","rgb","seg","image"});
    g.edges.push_back({"depth","cloud","pose","cloud"});
    g.edges.push_back({"seg","detections","pose","roi"});
    g.edges.push_back({"pose","pose","grasp","pose"});
    if(opts.with_robot){
        g.edges.push_back({"grasp","tcp","move","target"});
    }

    return g;
}
