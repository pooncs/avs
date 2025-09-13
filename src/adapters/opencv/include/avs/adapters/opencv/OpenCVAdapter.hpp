#pragma once
#if __has_include(<opencv2/core.hpp>)
  #include <opencv2/core.hpp>
  #define AVS_WITH_OPENCV_HEADERS 1
#else
  #define AVS_WITH_OPENCV_HEADERS 0
#endif
#include <cstdint>
#include <string>
#include <vector>

namespace avs { namespace adapters { namespace opencv {

class OpenCVAdapter final {
public:
    OpenCVAdapter() = default;

    static bool available() noexcept {
#if AVS_WITH_OPENCV_HEADERS
        return true;
#else
        return false;
#endif
    }

    static std::string version();

#if AVS_WITH_OPENCV_HEADERS
    // Creates a cv::Mat view over external memory. Caller keeps the data alive.
    static cv::Mat view(uint8_t* data, int width, int height, int channels, size_t strideBytes);

    // Deep copy to a contiguous blob.
    static std::vector<uint8_t> toBlob(const cv::Mat& m);

    static cv::Mat ensureGray(const cv::Mat& src); // 1-channel 8U
    static cv::Mat ensureBGR(const cv::Mat& src);  // 3-channel 8U
#endif
};

}}} // namespace avs::adapters::opencv
