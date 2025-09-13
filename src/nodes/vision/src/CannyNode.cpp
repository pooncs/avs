#include "avs/nodes/vision/CannyNode.hpp"
#include <opencv2/imgproc.hpp>

namespace avs { namespace vision {

cv::Mat CannyNode::run(const cv::Mat& in) const {
    if (in.empty()) return cv::Mat();
    cv::Mat gray; if (in.channels() == 1) gray = in; else cv::cvtColor(in, gray, cv::COLOR_BGR2GRAY);
    cv::Mat edges; cv::Canny(gray, edges, params_.t1, params_.t2, params_.aperture, params_.L2);
    return edges;
}

}} // namespace
