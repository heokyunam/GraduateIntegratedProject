#include <openni-nite/NiTE.h>
#include <OpenNI.h>
#include <iostream>

#define LOG(LEVEL) (libfreenect2::libfreenect2::LogMessage(::libfreenect2::getGlobalLogger(), ::libfreenect2::Logger::LEVEL, LOG_SOURCE).stream())

class SimpleTracker {
private:
    nite::UserTracker* tracker;
    openni::Device* device;
    nite::UserTrackerFrameRef* frame;
    bool captured;
public:
    SimpleTracker() {}
    ~SimpleTracker() {
        nite::NiTE::shutdown();
    }
    bool init(int userId = 0);
    bool update();
    bool isTracked(int userId = 0);
    bool filewrite(const char* filename, int userId = 0);
    const nite::Point3f& getJoint(int userId, nite::JointType joint);
};
