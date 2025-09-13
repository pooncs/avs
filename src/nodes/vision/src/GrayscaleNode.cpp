#include "avs/nodes/vision/GrayscaleNode.hpp"
#include "avs/adapters/opencv/OpenCVAdapter.hpp"
#include <opencv2/imgproc.hpp>

namespace avs { namespace vision {

cv::Mat GrayscaleNode::run(const cv::Mat& in) const {
    if (in.empty()) return cv::Mat();
    if (in.channels() == 1) return in;
    cv::Mat out;
    if (in.channels() == 3) {
        cv::cvtColor(in, out, cv::COLOR_BGR2GRAY);
    } else if (in.channels() == 4) {
        if (params_.keep_alpha) {
            std::vector<cv::Mat> ch; cv::split(in, ch);
            out = ch[3];
        } else {
            cv::cvtColor(in, out, cv::COLOR_BGRA2GRAY);
        }
    } else {
        out = in.clone();
    }
    return out;
}

}} // namespace
