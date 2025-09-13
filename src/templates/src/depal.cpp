
#include "avs/templates/depal.hpp"

using namespace avs::templates;

TemplateGraphSpec avs::templates::MakeDepalletizingTemplate(const TemplateOptions& opts){
    TemplateGraphSpec g; g.name = "depalettizing"; // spelling variant

    const bool use_rs = opts.prefer_realsense && !opts.prefer_photoneo;

    g.nodes.push_back({"cam", use_rs ? "camera.realsense.Capture" : "camera.photoneo.Capture", {}});
    g.nodes.push_back({"cloud", "vision.DepthToCloud", {}});
    g.nodes.push_back({"pallet", "geom.PlaneSegmentation", {{"axis","z"}}});
    g.nodes.push_back({"pile", "geom.BoxDetector", {}});
    g.nodes.push_back({"pick", "plan.LayeredPickPlanner", {{"pattern","grid"}}});
    if(opts.with_robot){
        g.nodes.push_back({"move", "robot.abb.MoveToPose", {{"speed", "80"}}});
    }

    g.edges.push_back({"cam","depth","cloud","depth"});
    g.edges.push_back({"cloud","cloud","pallet","cloud"});
    g.edges.push_back({"pallet","inliers","pile","cloud"});
    g.edges.push_back({"pile","boxes","pick","targets"});
    if(opts.with_robot){
        g.edges.push_back({"pick","tcp","move","target"});
    }

    return g;
}
