#include "avs/adapters/realsense/RealSenseFrameGrabber.hpp"
#include <mutex>
#include <cstring>

#if defined(WITH_REALSENSE)
#  include <librealsense2/rs.hpp>
#endif

namespace avs { namespace adapters { namespace realsense {

static RSIntrinsics to_intr(
#if defined(WITH_REALSENSE)
    const rs2_intrinsics& in
#else
    int
#endif
) {
    RSIntrinsics o; 
#if defined(WITH_REALSENSE)
    o.width=in.width; o.height=in.height; o.fx=in.fx; o.fy=in.fy; o.cx=in.ppx; o.cy=in.ppy; 
    if (in.coeffs) { for (int i=0;i<5 && i< (int)(sizeof(in.coeffs)/sizeof(in.coeffs[0])); ++i) o.dist[i]=in.coeffs[i]; }
#else
    (void)in;
#endif
    return o;
}

struct RealSenseFrameGrabber::Impl {
    std::shared_ptr<RealSenseDeviceManager> dev;
    RSConfig cfg{};
    bool run{false};
    std::mutex mtx;
#if defined(WITH_REALSENSE)
    rs2::pipeline pipe;
    rs2::config conf;
    rs2::pipeline_profile prof;
#endif
};

RealSenseFrameGrabber::RealSenseFrameGrabber(std::shared_ptr<RealSenseDeviceManager> d):impl(new Impl){impl->dev=d;}
RealSenseFrameGrabber::~RealSenseFrameGrabber(){stop();}

bool RealSenseFrameGrabber::start(const RSConfig& c){
    std::lock_guard<std::mutex> lk(impl->mtx);
    impl->cfg=c;
#if defined(WITH_REALSENSE)
    if(!impl->dev || !impl->dev->isOpen()) return false;
    impl->conf.disable_all_streams();
    if(c.enable_color) impl->conf.enable_stream(RS2_STREAM_COLOR, c.color_w, c.color_h, RS2_FORMAT_RGBA8, c.fps);
    if(c.enable_depth) impl->conf.enable_stream(RS2_STREAM_DEPTH, c.depth_w, c.depth_h, RS2_FORMAT_Z16, c.fps);
    impl->prof = impl->pipe.start(impl->conf);
#endif
    impl->run=true; return true;
}

void RealSenseFrameGrabber::stop(){ std::lock_guard<std::mutex> lk(impl->mtx);
#if defined(WITH_REALSENSE)
    if(impl->run) { try { impl->pipe.stop(); } catch(...){} }
#endif
    impl->run=false; }

bool RealSenseFrameGrabber::running() const { return impl->run; }

std::optional<RSFrame> RealSenseFrameGrabber::getColor(){
    std::lock_guard<std::mutex> lk(impl->mtx);
    if(!impl->run) return std::nullopt;
    RSFrame out; out.stream=RSStream::Color;
#if defined(WITH_REALSENSE)
    rs2::frameset fs = impl->pipe.wait_for_frames();
    rs2::video_frame c = fs.get_color_frame();
    if(!c) return std::nullopt;
    out.width=c.get_width(); out.height=c.get_height(); out.bpp=32; out.stride=c.get_stride_in_bytes();
    out.data.resize(size_t(out.stride*out.height)); std::memcpy(out.data.data(), c.get_data(), out.data.size());
    auto s = c.get_profile().as<rs2::video_stream_profile>();
    out.intr = to_intr(s.get_intrinsics());
#else
    out.width=2; out.height=2; out.bpp=32; out.stride=8;
    out.data = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,255,255};
#endif
    out.t = std::chrono::steady_clock::now();
    return out;
}

std::optional<RSFrame> RealSenseFrameGrabber::getDepth(){
    std::lock_guard<std::mutex> lk(impl->mtx);
    if(!impl->run) return std::nullopt;
    RSFrame out; out.stream=RSStream::Depth;
#if defined(WITH_REALSENSE)
    rs2::frameset fs = impl->pipe.wait_for_frames();
    rs2::depth_frame d = fs.get_depth_frame();
    if(!d) return std::nullopt;
    out.width=d.get_width(); out.height=d.get_height(); out.bpp=16; out.stride=d.get_stride_in_bytes();
    out.data.resize(size_t(out.stride*out.height)); std::memcpy(out.data.data(), d.get_data(), out.data.size());
    auto s = d.get_profile().as<rs2::video_stream_profile>();
    out.intr = to_intr(s.get_intrinsics());
#else
    out.width=2; out.height=2; out.bpp=16; out.stride=4; out.data.resize(8, 0);
#endif
    out.t = std::chrono::steady_clock::now();
    return out;
}

RSIntrinsics RealSenseFrameGrabber::colorIntrinsics() const {
#if defined(WITH_REALSENSE)
    try{
        auto sp = impl->prof.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
        return to_intr(sp.get_intrinsics());
    }catch(...){ return {}; }
#else
    return {};
#endif
}

RSIntrinsics RealSenseFrameGrabber::depthIntrinsics() const {
#if defined(WITH_REALSENSE)
    try{
        auto sp = impl->prof.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
        return to_intr(sp.get_intrinsics());
    }catch(...){ return {}; }
#else
    return {};
#endif
}

}}}
