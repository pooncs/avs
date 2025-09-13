#pragma once
#include <opencv2/core.hpp>
#include "avs/nodes/vision/Types.hpp"

namespace avs { namespace vision {

class TemplateMatchNode {
public:
    enum Method { SQDIFF = 0, SQDIFF_NORMED = 1, CCORR = 2, CCORR_NORMED = 3, CCOEFF = 4, CCOEFF_NORMED = 5 };
    struct Params { Method method{CCOEFF_NORMED}; };
    explicit TemplateMatchNode(Params p = {}) : params_(p) {}
    TemplateMatchResult run(const cv::Mat& image, const cv::Mat& templ) const;
    static const char* Name() { return "vision.template_match"; }
private:
    Params params_{};
};

}} // namespace
