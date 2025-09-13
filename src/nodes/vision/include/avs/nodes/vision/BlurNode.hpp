#pragma once
#include <opencv2/core.hpp>

namespace avs { namespace vision {

class BlurNode {
public:
    struct Params { int ksize{5}; double sigmaX{0.0}; double sigmaY{0.0}; };
    explicit BlurNode(Params p = {}) : params_(p) {}
    cv::Mat run(const cv::Mat& in) const; // same channels
    static const char* Name() { return "vision.blur"; }
private:
    Params params_{};
};

}} // namespace
