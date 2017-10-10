#include "simpletracker.h"
#include <fstream>

const char* enumToCharArray(nite::JointType type) {
    switch(type)
    {
        case nite::JOINT_HEAD:
            return "JOINT_HEAD";
        case nite::JOINT_NECK:
            return "JOINT_NECK";

        case nite::JOINT_LEFT_SHOULDER:
            return "JOINT_LEFT_SHOULDER";
        case nite::JOINT_RIGHT_SHOULDER:
            return "JOINT_RIGHT_SHOULDER";
        case nite::JOINT_LEFT_ELBOW:
            return "JOINT_LEFT_ELBOW";
        case nite::JOINT_RIGHT_ELBOW:
            return "JOINT_RIGHT_ELBOW";
        case nite::JOINT_LEFT_HAND:
            return "JOINT_LEFT_HAND";
        case nite::JOINT_RIGHT_HAND:
            return "JOINT_RIGHT_HAND";

        case nite::JOINT_TORSO:
            return "JOINT_TORSO";

        case nite::JOINT_LEFT_HIP:
            return "JOINT_LEFT_HIP";
        case nite::JOINT_RIGHT_HIP:
            return "JOINT_RIGHT_HIP";
        case nite::JOINT_LEFT_KNEE:
            return "JOINT_LEFT_KNEE";
        case nite::JOINT_RIGHT_KNEE:
            return "JOINT_RIGHT_KNEE";
        case nite::JOINT_LEFT_FOOT:
            return "JOINT_LEFT_FOOT";
        case nite::JOINT_RIGHT_FOOT:
            return "JOINT_RIGHT_FOOT";
        default:
            return "";
    }
}
/**
 * create device and init serial numbers by enumerateDevices
 * only complete userTracker
 */
bool SimpleTracker::init(int i)
{
    nite::Status niteStatus;
    openni::Status niStatus;
    openni::OpenNI::initialize();
	nite::NiTE::initialize();
    
    tracker = new nite::UserTracker();
    device = new openni::Device();
    openni::Array<openni::DeviceInfo> infoList;
    openni::OpenNI::enumerateDevices(&infoList);
    
    std::cout << "[SimpleTracker] size : " << infoList.getSize() << std::endl;
    std::cout << "[SimpleTracker] name : " << infoList[i].getName() << std::endl;
    std::cout << "[SimpleTracker] uri : " << infoList[i].getUri() << std::endl;
    niStatus = device->open(infoList[i].getUri());
    
    if(niStatus != openni::STATUS_OK)
    {
        std::cout << "[SimpleTracker] device intialize error" << std::endl;
        return false;
    }
    
    niteStatus = tracker->create(device);
    
    if(niteStatus != nite::STATUS_OK) 
    {
        std::cout << "[SimpleTracker] " << i << " tracker creation error" << std::endl;
        return false;//here it is
    }
    
    captured = false;
    return true;
}

bool SimpleTracker::update()
{
    nite::Status niteStatus;
    niteStatus = tracker->readFrame(&frame);
    if(niteStatus != nite::STATUS_OK) {
        std::cout << "[SimpleTracker] tracker reading error" << std::endl;
        return false;
    }
    
    const nite::Array<nite::UserData>& users = frame.getUsers();
    std::cout << "[SimpleTracker] tracking start" << std::endl;
    for(int i = 0; i < users.getSize(); i++)
    {
        const nite::UserData& user = users[i];
        if(user.isNew())
        {
            tracker->startSkeletonTracking(user.getId());
			tracker->startPoseDetection(user.getId(), nite::POSE_CROSSED_HANDS);
        }
    }
    std::cout << "[SimpleTracker] tracking start2" << std::endl;
    return true;
}

bool SimpleTracker::filewrite(const char* filename, int userId)
{
    bool captured = false;
    if(frame.getUsers().getSize() < userId) return false;
    const nite::UserData& user = frame.getUsers()[userId];
    std::ofstream out(filename);
    for(int i = 0; i < 15; i++)
    {
        const nite::Skeleton& skel = user.getSkeleton();
        const nite::SkeletonJoint& joint = skel.getJoint((nite::JointType)i);
        if(joint.getPositionConfidence() > .5) {
            std::cout << "drawing some.. " << joint.getType() << std::endl;
            const nite::Point3f& pos = joint.getPosition();
            out << "<joint>" << std::endl;
            out << "    <id>" << enumToCharArray(joint.getType()) << "</id>" << std::endl;
            out << "    <x>" << pos.x << "</x>" << std::endl;
            out << "    <y>" << pos.y << "</y>" << std::endl;
            out << "    <z>" << pos.z << "</z>" << std::endl;
            out << "    <data> " << std::endl << pos.x << ";" << pos.y << ";" << pos.z << std::endl << "    </data>" << std::endl;
            out << "</joint>" << std::endl;
            captured = true;
        }
        else {
            captured = false;
        }
    }
    out.close();
    return captured;
}

bool SimpleTracker::isTracked(int userId)
{
    return frame.getUsers().getSize() != 0 && 
    frame.getUsers()[userId].getSkeleton().getState() == nite::SKELETON_TRACKED;
}

const nite::Point3f& SimpleTracker::getJoint(int userId, nite::JointType joint)
{
    const nite::Skeleton& skel = frame.getUsers()[userId].getSkeleton();
    const nite::SkeletonJoint& jointData = skel.getJoint(joint);
    return jointData.getPosition();
}
