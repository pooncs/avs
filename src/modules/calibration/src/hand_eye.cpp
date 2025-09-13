
#include "calibration/hand_eye.hpp"

#ifdef WITH_OPENCV
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#endif

namespace avs::calibration {

static void to_cv(const PoseRT& in, cv::Mat& R, cv::Mat& t) {
#ifdef WITH_OPENCV
    R = cv::Mat(3,3,CV_64F);
    t = cv::Mat(3,1,CV_64F);
    for (int r=0;r<3;++r) for (int c=0;c<3;++c) R.at<double>(r,c) = in.R[r*3+c];
    for (int i=0;i<3;++i) t.at<double>(i,0) = in.t[i];
#endif
}

Result HandEyeCalibrator::compute(const std::vector<PoseRT>& ee2base,
                                  const std::vector<PoseRT>& target2cam,
                                  HandEyeMethod method,
                                  PoseRT& out_cam2ee) const noexcept {
#ifndef WITH_OPENCV
    (void)ee2base; (void)target2cam; (void)method; (void)out_cam2ee;
    return Result::failure("WITH_OPENCV=OFF: Hand-eye requires OpenCV calib3d");
#else
    if (ee2base.size() != target2cam.size() || ee2base.size() < 2) {
        return Result::failure("Input size mismatch or too few pairs");
    }
    std::vector<cv::Mat> R_gripper2base, t_gripper2base;
    std::vector<cv::Mat> R_target2cam_cv, t_target2cam_cv;
    R_gripper2base.reserve(ee2base.size());
    t_gripper2base.reserve(ee2base.size());
    R_target2cam_cv.reserve(target2cam.size());
    t_target2cam_cv.reserve(target2cam.size());

    for (size_t i=0;i<ee2base.size();++i) {
        cv::Mat R, t;
        to_cv(ee2base[i], R, t);
        R_gripper2base.push_back(R);
        t_gripper2base.push_back(t);
        to_cv(target2cam[i], R, t);
        R_target2cam_cv.push_back(R);
        t_target2cam_cv.push_back(t);
    }
    cv::Mat R_cam2gripper, t_cam2gripper;
    int cv_method = cv::CALIB_HAND_EYE_TSAI;
    switch (method) {
        case HandEyeMethod::TSAI: cv_method = cv::CALIB_HAND_EYE_TSAI; break;
        case HandEyeMethod::PARK: cv_method = cv::CALIB_HAND_EYE_PARK; break;
        case HandEyeMethod::DANIILIDIS: cv_method = cv::CALIB_HAND_EYE_DANIILIDIS; break;
    }
    try {
        cv::calibrateHandEye(
            R_gripper2base, t_gripper2base,
            R_target2cam_cv, t_target2cam_cv,
            R_cam2gripper, t_cam2gripper,
            cv_method
        );
    } catch (const cv::Exception& e) {
        return Result::failure(std::string("OpenCV exception: ")+ e.what());
    }
    // Pack output
    for (int r=0;r<3;++r) for (int c=0;c<3;++c) out_cam2ee.R[r*3+c] = R_cam2gripper.at<double>(r,c);
    for (int i=0;i<3;++i) out_cam2ee.t[i] = t_cam2gripper.at<double>(i,0);
    return Result::success();
#endif
}

} // namespace avs::calibration
