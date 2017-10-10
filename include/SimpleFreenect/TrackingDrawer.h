#include <GL/glut.h>
#include <openni2/OpenNI.h>
#include <NiTE.h>

using namespace openni;

#define GL_WIN_SIZE_X	1920
#define GL_WIN_SIZE_Y	1080
#define TEXTURE_SIZE	540
#define MAX_DEPTH 10000

class TrackingDrawer {  
private:
	RGB888Pixel*		m_pTexMap;
	openni::VideoFrameRef depthFrame;
    int g_nXRes, g_nYRes;
    unsigned int		m_nTexMapX;
	unsigned int		m_nTexMapY;
	float				m_pDepthHist[MAX_DEPTH];
    nite::UserTrackerFrameRef* userTrackerFrame;
    int x, y;
public:
    TrackingDrawer() {}
    openni::Status InitOpenGL(int argc, char **argv);
    static void BeginScene();
    static void EndScene();
    void Init(nite::UserTrackerFrameRef* userTrackerFrame, int x, int y);
    void Draw();
    void DrawLimb(nite::UserTracker* pUserTracker, const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2, int color);
    void DrawSkeleton(nite::UserTracker* pUserTracker, const nite::UserData& userData);
};
