
#include "simpleprotonect.h"

SimpleFreenect::SimpleFreenect()
{
}

int SimpleFreenect::start(bool enable_rgb, bool enable_depth)
{
    this->undistorted = new libfreenect2::Frame(512, 424, 4);
    this->registered = new libfreenect2::Frame(512, 424, 4);        
    
    int types = 0;
    if (enable_rgb)
        types |= libfreenect2::Frame::Color;
    if (enable_depth)
        types |= FRAME_IR | FRAME_DEPTH;
    listener = new libfreenect2::SyncMultiFrameListener(types);
    dev->setColorFrameListener(listener);
    dev->setIrAndDepthFrameListener(listener);
    
    
    if (enable_rgb && enable_depth)
    {
        if (!dev->start()){
            std::cout << "error occur!" << std::endl;
            return -1;
        }
    }
    else if (!dev->startStreams(enable_rgb, enable_depth))
    {        
        return -1;
    }
    
    registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    
}

int SimpleFreenect::initDevice(int method, int index)
{
    if(method == METHOD_GL)//Change == -> compare by Hanwool
    {
#ifdef LIBFREENECT2_WITH_OPENGL_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::OpenGLPacketPipeline();
#else
      std::cout << "OpenGL pipeline is not supported!" << std::endl;
#endif
    }
    if(freenect2.enumerateDevices() == 0)
    {
        std::cout << "no device connected!" << std::endl;
        return -1;
    }

    serial = freenect2.getDeviceSerialNumber(index);
    if(pipeline)
        dev = freenect2.openDevice(serial, pipeline);
    else
        dev = freenect2.openDevice(serial);    
    
    if(dev == 0)
    {
        std::cout << "failure opening device!" << std::endl;
        return -1;
    }
    return 0;
}


libfreenect2::Frame* SimpleFreenect::getFrame(libfreenect2::Frame::Type index)
{
    return this->frames[index];
}


int SimpleFreenect::view(Viewer& viewer, bool enable_rgb, bool enable_depth){
    if(!listener->waitForNewFrame(frames, 10*1000))
    {
	    std::cout << "[simpleprotonect] time out!" << std::endl;
	    return -1;
    }
    
    libfreenect2::Frame* rgb = frames[FRAME_COLOR];
    libfreenect2::Frame* ir = frames[FRAME_IR];
    libfreenect2::Frame* depth = frames[FRAME_DEPTH];

    registration->apply(rgb, depth, undistorted, registered);
    
    if (enable_rgb)
    {
      viewer.addFrame("RGB", rgb);
    }
    if (enable_depth)
    {
      viewer.addFrame("ir", ir);
      viewer.addFrame("depth", depth);
    }
    if (enable_rgb && enable_depth)
    {
      viewer.addFrame("registered", registered);
    }
    bool returnVal = viewer.render();
    listener->release(frames);
    return returnVal;
}

void SimpleFreenect::release()
{
    listener->release(frames);
    dev->stop();
    dev->close();
    delete registration;
}
