
        #include "calibration/extrinsics.hpp"
        #include <sstream>
        #include <fstream>

        #ifdef WITH_OPENCV
        #include <opencv2/core.hpp>
        #include <opencv2/calib3d.hpp>
        #endif

        namespace avs::calibration {

        std::string to_json(const PoseRT& p) {
            std::ostringstream ss;
            ss.setf(std::ios::fixed); ss.precision(9);
            ss << "{
  "R": [";
            for (int i=0;i<9;++i) { ss << p.R[i]; if (i<8) ss << ", "; }
            ss << "],
  "t": [" << p.t[0] << ", " << p.t[1] << ", " << p.t[2] << "]
}";
            return ss.str();
        }

        bool from_json(const std::string& s, PoseRT& p) {
            // Minimal parser: expects exact fields. Not robust; sufficient for internal use.
            auto rpos = s.find(""R"");
            auto tpos = s.find(""t"");
            if (rpos == std::string::npos || tpos == std::string::npos) return false;
            auto lb1 = s.find('[', rpos); auto rb1 = s.find(']', lb1);
            auto lb2 = s.find('[', tpos); auto rb2 = s.find(']', lb2);
            if (lb1==std::string::npos||rb1==std::string::npos||lb2==std::string::npos||rb2==std::string::npos) return false;
            std::istringstream rs(s.substr(lb1+1, rb1-lb1-1));
            std::istringstream ts(s.substr(lb2+1, rb2-lb2-1));
            for (int i=0;i<9;++i){ char comma; rs >> p.R[i]; if (i<8) rs >> comma; }
            for (int i=0;i<3;++i){ char comma; ts >> p.t[i]; if (i<2) ts >> comma; }
            return true;
        }

        Result save_pose(const std::string& file_path, const PoseRT& p) {
            std::ofstream ofs(file_path, std::ios::binary);
            if (!ofs) return Result::failure("Failed to open file for write");
            ofs << to_json(p);
            return Result::success();
        }

        Result load_pose(const std::string& file_path, PoseRT& p) {
            std::ifstream ifs(file_path, std::ios::binary);
            if (!ifs) return Result::failure("Failed to open file for read");
            std::ostringstream ss; ss << ifs.rdbuf();
            if (!from_json(ss.str(), p)) return Result::failure("Parse error");
            return Result::success();
        }

        Result estimate_base_to_camera_pnp(const std::vector<std::array<double,3>>& object_pts,
                                           const std::vector<std::array<double,2>>& image_pts,
                                           const std::array<double,9>& K,
                                           const std::vector<double>& dist,
                                           PoseRT& out_base2cam) noexcept {
        #ifndef WITH_OPENCV
            (void)object_pts; (void)image_pts; (void)K; (void)dist; (void)out_base2cam;
            return Result::failure("WITH_OPENCV=OFF: PnP requires OpenCV calib3d");
        #else
            if (object_pts.size() < 4 || object_pts.size() != image_pts.size()) {
                return Result::failure("Need >=4 correspondences and matching sizes");
            }
            std::vector<cv::Point3d> obj; obj.reserve(object_pts.size());
            std::vector<cv::Point2d> img; img.reserve(image_pts.size());
            for (size_t i=0;i<object_pts.size();++i) {
                obj.emplace_back(object_pts[i][0], object_pts[i][1], object_pts[i][2]);
                img.emplace_back(image_pts[i][0], image_pts[i][1]);
            }
            cv::Mat Kcv(3,3,CV_64F), distcv;
            for (int r=0;r<3;++r) for (int c=0;c<3;++c) Kcv.at<double>(r,c) = K[r*3+c];
            distcv = cv::Mat(dist).clone();
            if (distcv.empty()) distcv = cv::Mat::zeros(1, 5, CV_64F);
            cv::Mat rvec, tvec;
            bool ok = cv::solvePnP(obj, img, Kcv, distcv, rvec, tvec, false, cv::SOLVEPNP_ITERATIVE);
            if (!ok) return Result::failure("solvePnP failed");
            cv::Mat R;
            cv::Rodrigues(rvec, R);
            for (int r=0;r<3;++r) for (int c=0;c<3;++c) out_base2cam.R[r*3+c] = R.at<double>(r,c);
            for (int i=0;i<3;++i) out_base2cam.t[i] = tvec.at<double>(i,0);
            return Result::success();
        #endif
        }

        } // namespace avs::calibration
