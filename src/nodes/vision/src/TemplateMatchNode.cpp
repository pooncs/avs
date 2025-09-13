#include "avs/nodes/vision/TemplateMatchNode.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace avs { namespace vision {

static int toCVMethod(TemplateMatchNode::Method m) {
    switch (m) {
        case TemplateMatchNode::SQDIFF: return cv::TM_SQDIFF;
        case TemplateMatchNode::SQDIFF_NORMED: return cv::TM_SQDIFF_NORMED;
        case TemplateMatchNode::CCORR: return cv::TM_CCORR;
        case TemplateMatchNode::CCORR_NORMED: return cv::TM_CCORR_NORMED;
        case TemplateMatchNode::CCOEFF: return cv::TM_CCOEFF;
        case TemplateMatchNode::CCOEFF_NORMED: return cv::TM_CCOEFF_NORMED;
    }
    return cv::TM_CCOEFF_NORMED;
}

TemplateMatchResult TemplateMatchNode::run(const cv::Mat& image, const cv::Mat& templ) const {
    TemplateMatchResult r;
    if (image.empty() || templ.empty()) return r;
    cv::Mat imgGray, tplGray;
    if (image.channels() == 1) imgGray = image; else cv::cvtColor(image, imgGray, cv::COLOR_BGR2GRAY);
    if (templ.channels() == 1) tplGray = templ; else cv::cvtColor(templ, tplGray, cv::COLOR_BGR2GRAY);

    const int result_cols = imgGray.cols - tplGray.cols + 1;
    const int result_rows = imgGray.rows - tplGray.rows + 1;
    if (result_cols <= 0 || result_rows <= 0) return r;
    r.response.create(result_rows, result_cols, CV_32FC1);

    cv::matchTemplate(imgGray, tplGray, r.response, toCVMethod(params_.method));
    cv::normalize(r.response, r.response, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    double minVal = 0.0, maxVal = 0.0; cv::Point minLoc, maxLoc;
    cv::minMaxLoc(r.response, &minVal, &maxVal, &minLoc, &maxLoc);
    r.minVal = minVal; r.maxVal = maxVal; r.minLoc = minLoc; r.maxLoc = maxLoc;
    return r;
}

}} // namespace
