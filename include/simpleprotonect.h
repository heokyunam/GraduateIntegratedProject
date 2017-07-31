
#include <iostream>
#include <string>
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
#include <libfreenect2/registration.h>
#include "viewer.h"

#define FRAME_COLOR libfreenect2::Frame::Color
#define FRAME_IR libfreenect2::Frame::Ir
#define FRAME_DEPTH libfreenect2::Frame::Depth
#define METHOD_GL 1001

class SimpleFreenect {
private:
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device *dev;
    libfreenect2::PacketPipeline *pipeline;
    libfreenect2::FrameMap frames;
    libfreenect2::SyncMultiFrameListener* listener;
    libfreenect2::Freenect2Device::IrCameraParams ir;
    libfreenect2::Registration* registration;
    libfreenect2::Frame *undistorted, *registered;
    std::string serial;
public:
    SimpleFreenect();
    int initDevice(int method, int index);
    int start(bool enable_rgb, bool enable_depth);
    bool waitForNewFrame();
    libfreenect2::Frame* getFrame(libfreenect2::Frame::Type index);
    int view(Viewer& viewer, bool enable_rgb, bool enable_depth);
    void release();
    
    
};

