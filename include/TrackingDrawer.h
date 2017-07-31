#include <GL/glut.h>
#include <openni2/OpenNI.h>
#include <NiTE.h>

using namespace openni;

#define GL_WIN_SIZE_X	1920
#define GL_WIN_SIZE_Y	1080
#define TEXTURE_SIZE	540

class TrackingDrawer {  
private:
	RGB888Pixel*		m_pTexMap;
    int g_nXRes, g_nYRes;
public:
    TrackingDrawer() {}
    openni::Status InitOpenGL(int argc, char **argv);
    void Display();
    void Run();
};
