#include "avs/adapters/opencv/OpenCVAdapter.hpp"
#if AVS_WITH_OPENCV_HEADERS
#include <opencv2/imgproc.hpp>
#endif
#include <sstream>

namespace avs { namespace adapters { namespace opencv {

std::string OpenCVAdapter::version() {
#if AVS_WITH_OPENCV_HEADERS
    std::ostringstream oss;
    oss << CV_VERSION;
    return oss.str();
#else
    return "OpenCV headers not found";
#endif
}

#if AVS_WITH_OPENCV_HEADERS
cv::Mat OpenCVAdapter::view(uint8_t* data, int width, int height, int channels, size_t strideBytes) {
    int type = 0;
    switch (channels) {
        case 1: type = CV_8UC1; break;
        case 2: type = CV_8UC2; break;
        case 3: type = CV_8UC3; break;
        case 4: type = CV_8UC4; break;
        default: type = CV_8UC1; break;
    }
    return cv::Mat(height, width, type, data, strideBytes);
}

std::vector<uint8_t> OpenCVAdapter::toBlob(const cv::Mat& m) {
    cv::Mat c = m.isContinuous() ? m : m.clone();
    std::vector<uint8_t> blob(c.total() * c.elemSize());
    std::memcpy(blob.data(), c.data, blob.size());
    return blob;
}

cv::Mat OpenCVAdapter::ensureGray(const cv::Mat& src) {
    if (src.channels() == 1) return src;
    cv::Mat gray;
    switch (src.channels()) {
        case 3: cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY); break;
        case 4: cv::cvtColor(src, gray, cv::COLOR_BGRA2GRAY); break;
        default: gray = src.clone(); break;
    }
    return gray;
}

cv::Mat OpenCVAdapter::ensureBGR(const cv::Mat& src) {
    if (src.channels() == 3) return src;
    cv::Mat bgr;
    switch (src.channels()) {
        case 1: cv::cvtColor(src, bgr, cv::COLOR_GRAY2BGR); break;
        case 4: cv::cvtColor(src, bgr, cv::COLOR_BGRA2BGR); break;
        default: bgr = src.clone(); break;
    }
    return bgr;
}
#endif

}}} // namespace
