
#include "avs/templates/inspection.hpp"

using namespace avs::templates;

TemplateGraphSpec avs::templates::MakeInspectionTemplate(const TemplateOptions& opts){
    (void)opts; // unused
    TemplateGraphSpec g; g.name = "inspection";

    g.nodes.push_back({"cam", "camera.generic.Capture", {}});
    g.nodes.push_back({"pre", "vision.ImagePreprocess", {{"blur","gauss3"}}});
    g.nodes.push_back({"infer", "ml.DefectSegmentor", {{"model","seg.onnx"}}});
    g.nodes.push_back({"post", "vision.BinaryPost", {{"thresh","0.5"}}});
    g.nodes.push_back({"measure", "qc.DimMeasure", {{"cad","part.step"}}});
    g.nodes.push_back({"report", "qc.Report", {{"fmt","csv"}}});

    g.edges.push_back({"cam","image","pre","image"});
    g.edges.push_back({"pre","image","infer","image"});
    g.edges.push_back({"infer","mask","post","mask"});
    g.edges.push_back({"post","blobs","measure","rois"});
    g.edges.push_back({"measure","results","report","results"});

    return g;
}
