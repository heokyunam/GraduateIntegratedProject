/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2011 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

/** @file Protonect.cpp Main application file. */

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <string>
#include <unistd.h>

/// [headers]
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
#include <opencv2/opencv.hpp>

#include <NiTE.h>
#include <openni-nite/NiTE.h>

/// [headers]
//#ifdef EXAMPLES_WITH_OPENGL_SUPPORT
#include "viewer.h"
//#endif

std::vector<cv::Point3f> Generate3DPoints();
std::vector<cv::Point2f> Generate2DPoints();
bool protonect_shutdown = false; ///< Whether the running application should shut down.

void sigint_handler(int s)
{
  protonect_shutdown = true;
}

bool protonect_paused = false;
libfreenect2::Freenect2Device *devtopause;

//Doing non-trivial things in signal handler is bad. If you want to pause,
//do it in another thread.
//Though libusb operations are generally thread safe, I cannot guarantee
//everything above is thread safe when calling start()/stop() while
//waitForNewFrame().
void sigusr1_handler(int s)
{
  if (devtopause == 0)
    return;
/// [pause]
  if (protonect_paused)
    devtopause->start();
  else
    devtopause->stop();
  protonect_paused = !protonect_paused;
/// [pause]
}

//The following demostrates how to create a custom logger
/// [logger]
#include <fstream>
#include <cstdlib>
class MyFileLogger: public libfreenect2::Logger
{
private:
  std::ofstream logfile_;
public:
  MyFileLogger(const char *filename)
  {
    if (filename)
      logfile_.open(filename);
    level_ = Debug;
  }
  bool good()
  {
    return logfile_.is_open() && logfile_.good();
  }
  virtual void log(Level level, const std::string &message)
  {
    logfile_ << "[" << libfreenect2::Logger::level2str(level) << "] " << message << std::endl;
  }
};
/// [logger]

/// [main]
/**
 * Main application entry point.
 *
 * Accepted argumemnts:
 * - cpu Perform depth processing with the CPU.
 * - gl  Perform depth processing with OpenGL.
 * - cl  Perform depth processing with OpenCL.
 * - <number> Serial number of the device to open.
 * - -noviewer Disable viewer window.
 */

int main(int argc, char *argv[])
/// [main]
{
  std::string program_path(argv[0]);
  std::cerr << "Version: " << LIBFREENECT2_VERSION << std::endl;
  std::cerr << "Environment variables: LOGFILE=<protonect.log>" << std::endl;
  std::cerr << "Usage: " << program_path << " [-gpu=<id>] [gl | cl | clkde | cuda | cudakde | cpu] [<device serial>]" << std::endl;
  std::cerr << "        [-noviewer] [-norgb | -nodepth] [-help] [-version]" << std::endl;
  std::cerr << "        [-frames <number of frames to process>]" << std::endl;
  std::cerr << "To pause and unpause: pkill -USR1 Protonect" << std::endl;
  size_t executable_name_idx = program_path.rfind("Protonect");

  std::string binpath = "/";

  if(executable_name_idx != std::string::npos)
  {
    binpath = program_path.substr(0, executable_name_idx);
  }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  // avoid flooing the very slow Windows console with debug messages
  libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::Info));
#else
  // create a console logger with debug level (default is console logger with info level)
/// [logging]
  libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::Debug));
/// [logging]
#endif
/// [file logging]
  MyFileLogger *filelogger = new MyFileLogger(getenv("LOGFILE"));
  if (filelogger->good())
    libfreenect2::setGlobalLogger(filelogger);
  else
    delete filelogger;
/// [file logging]

/// [context]
  libfreenect2::Freenect2 freenect2;
  libfreenect2::Freenect2Device *dev = 0;
  libfreenect2::PacketPipeline *pipeline = 0;
  libfreenect2::Freenect2 freenect3;
  libfreenect2::Freenect2Device *dev2 = 0;
  libfreenect2::PacketPipeline *pipeline2 = 0;
/// [context]

  std::string serial = "";
  std::string serial2 = "";

  bool viewer_enabled = true;
  bool enable_rgb = true;
  bool enable_depth = true;
  bool enable_rgb2 = true;
  bool enable_depth2 = true;
  int deviceId = -1;
  size_t framemax = -1;
  libfreenect2::Freenect2Device::IrCameraParams ir2;
  libfreenect2::Freenect2Device::IrCameraParams ir1;

  for(int argI = 1; argI < argc; ++argI)
  {
    const std::string arg(argv[argI]);

    if(arg == "-help" || arg == "--help" || arg == "-h" || arg == "-v" || arg == "--version" || arg == "-version")
    {
      // Just let the initial lines display at the beginning of main
      return 0;
    }
    else if(arg.find("-gpu=") == 0)
    {
      if (pipeline)
      {
        std::cerr << "-gpu must be specified before pipeline argument" << std::endl;
        return -1;
      }
      deviceId = atoi(argv[argI] + 5);
    }
    else if(arg == "cpu")
    {
      if(!pipeline)
/// [pipeline]
        pipeline = new libfreenect2::CpuPacketPipeline();
/// [pipeline]
    }
    else if(arg == "gl")
    {
#ifdef LIBFREENECT2_WITH_OPENGL_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::OpenGLPacketPipeline();
      if(!pipeline2)
          pipeline2 = new libfreenect2::OpenGLPacketPipeline();
#else
      std::cout << "OpenGL pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "cl")
    {
#ifdef LIBFREENECT2_WITH_OPENCL_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::OpenCLPacketPipeline(deviceId);
#else
      std::cout << "OpenCL pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "clkde")
    {
#ifdef LIBFREENECT2_WITH_OPENCL_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::OpenCLKdePacketPipeline(deviceId);
#else
      std::cout << "OpenCL pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "cuda")
    {
#ifdef LIBFREENECT2_WITH_CUDA_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::CudaPacketPipeline(deviceId);
#else
      std::cout << "CUDA pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "cudakde")
    {
#ifdef LIBFREENECT2_WITH_CUDA_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::CudaKdePacketPipeline(deviceId);
#else
      std::cout << "CUDA pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg.find_first_not_of("0123456789") == std::string::npos) //check if parameter could be a serial number
    {
      serial = arg;
    }
    else if(arg == "-noviewer" || arg == "--noviewer")
    {
      viewer_enabled = false;
    }
    else if(arg == "-norgb" || arg == "--norgb")
    {
      enable_rgb = false;
    }
    else if(arg == "-nodepth" || arg == "--nodepth")
    {
      enable_depth = false;
    }
    else if(arg == "-frames")
    {
      ++argI;
      framemax = strtol(argv[argI], NULL, 0);
      if (framemax == 0) {
        std::cerr << "invalid frame count '" << argv[argI] << "'" << std::endl;
        return -1;
      }
    }
    else
    {
      std::cout << "Unknown argument: " << arg << std::endl;
    }
  }

  if (!enable_rgb && !enable_depth)
  {
    std::cerr << "Disabling both streams is not allowed!" << std::endl;
    return -1;
  }

/// [discovery]
  if(freenect2.enumerateDevices() == 0)
  {
    std::cout << "no device connected2!" << std::endl;
    return -1;
  }
  if(freenect3.enumerateDevices() == 0)
  {
    std::cout << "no device connected3!" << std::endl;
    return -1;
  }

  if (serial == "")
  {
    serial = freenect2.getDeviceSerialNumber(0);
    serial2 = freenect3.getDeviceSerialNumber(1);
    std::cout << serial << "\t" << serial2 << std::endl;
  }
/// [discovery]

  if(pipeline && pipeline2)
  {
/// [open]
    dev = freenect2.openDevice(serial, pipeline);
    dev2 = freenect3.openDevice(serial2, pipeline2);
/// [open]
  }
  else
  {
     dev = freenect2.openDevice(serial);
    dev2 = freenect3.openDevice(serial2);
  }

  if(dev == 0)
  {
    std::cout << "failure opening device1!" << std::endl;
    return -1;
  }
  else
  {
    std::cout << "success opening device1!" << std::endl;
  }
  
  if(dev2 == 0)
  {
    std::cout << "failure opening device2!" << std::endl;
    return -1;
  }
  else
  {
    std::cout << "success opening device2!" << std::endl;
  }
  
  devtopause = dev;

  signal(SIGINT,sigint_handler);
#ifdef SIGUSR1
  signal(SIGUSR1, sigusr1_handler);
#endif
  protonect_shutdown = false;

/// [listeners]
  int types = 0;
  if (enable_rgb)
    types |= libfreenect2::Frame::Color;
  if (enable_depth)
    types |= libfreenect2::Frame::Ir | libfreenect2::Frame::Depth;
  libfreenect2::SyncMultiFrameListener listener(types);
  libfreenect2::SyncMultiFrameListener listener2(types);
  libfreenect2::FrameMap frames;
  libfreenect2::FrameMap frames2;

  dev->setColorFrameListener(&listener);
  dev->setIrAndDepthFrameListener(&listener);
  dev2->setColorFrameListener(&listener2);
  dev2->setIrAndDepthFrameListener(&listener2);
/// [listeners]

/// [start]
  if (enable_rgb && enable_depth)
  {
    if (!dev->start()){
        std::cout << "error occur!" << std::endl;
        return -1;
    }
    
    std::cout << "device 1 started" << std::endl;
    
    if (!dev2->start()){
        std::cout << "error occur2!" << std::endl;
        return -1;
    }
    
    std::cout << "device 2 started" << std::endl;
  }
  else
  {
    if (!dev->startStreams(enable_rgb, enable_depth) || !dev2->startStreams(enable_rgb,enable_depth))
      return -1;
  }

  std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
  std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
  std::cout << "device serial: " << dev2->getSerialNumber() << std::endl;
  std::cout << "device firmware: " << dev2->getFirmwareVersion() << std::endl;
/// [start]

/// [registration setup]
  libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
  libfreenect2::Registration* registration2 = new libfreenect2::Registration(dev2->getIrCameraParams(), dev2->getColorCameraParams());

  libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4);
  libfreenect2::Frame undistorted2(512, 424, 4), registered2(512, 424, 4);
/// [registration setup]

  size_t framecount = 0;
  
#ifdef EXAMPLES_WITH_OPENGL_SUPPORT
  Viewer viewer;
  if (viewer_enabled)
    viewer.initialize();
  
#else
  viewer_enabled = false;
#endif

uint32_t initial_time1;
uint32_t initial_time2;

if (!listener.waitForNewFrame(frames, 10*1000)) // 10 sconds
{
  std::cout << "timeout!" << std::endl;
  return -1;
}

if (!listener2.waitForNewFrame(frames2, 10*1000)) // 10 sconds
{
  std::cout << "timeout!" << std::endl;
  return -1;
}

initial_time1 = frames[libfreenect2::Frame::Color]->timestamp;
initial_time2 = frames2[libfreenect2::Frame::Color]->timestamp;

listener.release(frames);
listener2.release(frames2);
/// [loop start]
int imagecounts = 0;
  while(!protonect_shutdown && (framemax == (size_t)-1 || framecount < framemax))
  {
    if (!listener.waitForNewFrame(frames, 10*1000)) // 10 sconds
    {
      std::cout << "timeout!" << std::endl;
      return -1;
    }
    libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
    libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
    libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];
    cv::Mat rgbMat;
    std::stringstream ss;
    std::stringstream ss2;
    cv::String filename;
    
    if (!listener2.waitForNewFrame(frames2, 10*1000))
    {
      std::cout << "timeout!" << std::endl;
      return -1;
    }
    libfreenect2::Frame *rgb2 = frames2[libfreenect2::Frame::Color];
    libfreenect2::Frame *ir2 = frames2[libfreenect2::Frame::Ir];
    libfreenect2::Frame *depth2 = frames2[libfreenect2::Frame::Depth];
    libfreenect2::Frame *raw2 = rgb2;

/// [loop start]
    //std::cout << "rgb1 timestamp : " << rgb->timestamp - initial_time1 << std::endl;
    //std::cout << "rgb2 timestamp : " << rgb2->timestamp - initial_time2 << std::endl;
    uint32_t diff = (rgb->timestamp - initial_time1) - (rgb2->timestamp - initial_time2);
    //std::cout << "diff : " << diff << std::endl;
    if (enable_rgb && enable_depth)
    {
/// [registration]
      registration->apply(rgb, depth, &undistorted, &registered);
      registration2->apply(rgb2, depth2, &undistorted2, &registered2);
/// [registration]
    }

    framecount++;
    if (!viewer_enabled)
    {
      if (framecount % 100 == 0)
        std::cout << "The viewer is turned off. Received " << framecount << " frames. Ctrl-C to stop." << std::endl;
      listener.release(frames);
      listener2.release(frames2);
      continue;
    }
#ifdef EXAMPLES_WITH_OPENGL_SUPPORT
    //written by heokyunam : viewer setting is here
    if (enable_rgb)
    {
      viewer.addFrame("RGB", rgb);
      viewer.addFrame("RGB2", rgb2);
      

    if(viewer.isImageCapture()){
        std::cout<< imagecounts << "----------------------------------------------------------------------------------------------"<<std::endl;
        cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbMat);
        ss << "images/"<< serial << "/test" << imagecounts << ".bmp";
        filename = ss.str();
        cv::imwrite(filename.c_str(), rgbMat);
        
        cv::Mat(rgb2->height, rgb2->width, CV_8UC4, rgb2->data).copyTo(rgbMat);
        ss2 << "images/"<< serial2 << "/test" << imagecounts << ".bmp";
        filename = ss2.str();
        cv::imwrite(filename.c_str(), rgbMat);
        
        imagecounts++;
        viewer.setImageCapture(false);
    }

    }
    if (enable_depth)
    {
      //viewer.addFrame("depth", depth);
      //viewer.addFrame("ir", ir);
      //viewer.addFrame("depth2", depth2);
      //viewer.addFrame("ir2", ir2);
    }
    if (enable_rgb && enable_depth)
    {
      viewer.addFrame("registered", &registered);
      viewer.addFrame("registered2", &registered2);
    }

    protonect_shutdown = protonect_shutdown || viewer.render();
#endif

/// [loop end]
    listener.release(frames);
    listener2.release(frames2);
    /** libfreenect2::this_thread::sleep_for(libfreenect2::chrono::milliseconds(100)); */
  }
/// [loop end]
/// [auto image list create start]
    std::ofstream outFile("test.xml");
    outFile << "<?xml version=\"1.0\"?>" << std::endl;
    outFile << "<opencv_storage>" << std::endl;
    outFile << "<imagelist>" << std::endl;
    for(int i = 0; i < imagecounts; i++){
        outFile << "\"images/"<< serial << "/test" << i << ".bmp\"" << std::endl;
        outFile << "\"images/"<< serial2 << "/test" << i << ".bmp\"" << std::endl;
    }
    outFile << "</imagelist>" << std::endl;
    outFile << "</opencv_storage>" << std::endl;
    outFile.close();
/// [auto image list create end]
  // TODO: restarting ir stream doesn't work!
  // TODO: bad things will happen, if frame listeners are freed before dev->stop() :(
/// [stop]
  dev->stop();
  dev->close();
  dev2->stop();
  dev2->close();
/// [stop]

  delete registration;

  return 0;
}
