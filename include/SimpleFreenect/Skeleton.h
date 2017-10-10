#include <NiTE.h>
using namespace nite;

class SFSkeleton
{
private:
    nite::UserTrackerFrameRef frame;
    int g_nXRes, g_nYRes;
    int x,y;
public:
    SFSkeleton() {}
    void init(nite::UserTrackerFrameRef userTrackerFrame, int x, int y) {      
        openni::VideoFrameRef depthFrame = userTrackerFrame.getDepthFrame();
        this->frame = userTrackerFrame;
        this->x = x;
        this->y = y;
        this->g_nXRes = depthFrame.getVideoMode().getResolutionX();
        this->g_nYRes = depthFrame.getVideoMode().getResolutionY();
    }
    void DrawSkeleton(nite::UserTracker* pUserTracker, const nite::UserData& userData);
    void DrawLimb(nite::UserTracker* pUserTracker, 
                  const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2, int color);
};

/*
 * examples:
 * SFSkeleton sk;
 * sk.init(userTrackerFrame2, 10, 0);//made by heokyunam
 * sk.DrawSkeleton(m_pUserTracker2, user);//made by heokyunam
 * niViewTest searching keyword : heokyunam
 * It's Successful
 */
