#include "avs/nodes/vision/BlurNode.hpp"
#include <opencv2/imgproc.hpp>

namespace avs { namespace vision {

cv::Mat BlurNode::run(const cv::Mat& in) const {
    if (in.empty()) return cv::Mat();
    int k = params_.ksize; if (k % 2 == 0) k += 1; if (k <= 0) k = 1;
    cv::Mat out; cv::GaussianBlur(in, out, cv::Size(k, k), params_.sigmaX, params_.sigmaY, cv::BORDER_DEFAULT);
    return out;
}

}} // namespace
