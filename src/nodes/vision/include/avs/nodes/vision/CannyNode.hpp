#pragma once
#include <opencv2/core.hpp>

namespace avs { namespace vision {

class CannyNode {
public:
    struct Params { double t1{50.0}; double t2{150.0}; int aperture{3}; bool L2{false}; };
    explicit CannyNode(Params p = {}) : params_(p) {}
    cv::Mat run(const cv::Mat& in) const; // 8U1 edges
    static const char* Name() { return "vision.canny"; }
private:
    Params params_{};
};

}} // namespace
