#pragma once
#include <string>
#include <opencv2/core.hpp>

namespace avs { namespace vision {

struct TemplateMatchResult {
    cv::Mat response;   // CV_32F map
    cv::Point maxLoc{}; // best location
    cv::Point minLoc{}; // worst location
    double maxVal{0.0};
    double minVal{0.0};
};

}} // namespace avs::vision
