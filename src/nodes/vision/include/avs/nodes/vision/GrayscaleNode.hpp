#pragma once
#include <opencv2/core.hpp>
#include <string>

namespace avs { namespace vision {

class GrayscaleNode {
public:
    struct Params { bool keep_alpha{false}; };
    explicit GrayscaleNode(Params p = {}) : params_(p) {}
    cv::Mat run(const cv::Mat& in) const; // 8U1 out
    static const char* Name() { return "vision.grayscale"; }
private:
    Params params_{};
};

}} // namespace
