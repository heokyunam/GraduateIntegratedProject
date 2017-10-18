#include <UserTrackingTest.h>

/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - User Viewer Sample                                   *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/
#if (defined _WIN32)
#define PRIu64 "llu"
#else
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif

#include "niViewerTest.h"
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <opencv2/opencv.hpp>
#include <OniCTypes.h>
#include <math.h>


#define GL_WIN_SIZE_X	1600
#define GL_WIN_SIZE_Y	900
#define TEXTURE_SIZE	540

#define DEFAULT_DISPLAY_MODE	DISPLAY_MODE_DEPTH

#define MIN_NUM_CHUNKS(data_size, chunk_size)	((((data_size)-1) / (chunk_size) + 1))
#define MIN_CHUNKS_SIZE(data_size, chunk_size)	(MIN_NUM_CHUNKS(data_size, chunk_size) * (chunk_size))

SampleViewer* SampleViewer::ms_self = NULL;

bool g_drawSkeleton = true;
bool g_drawCenterOfMass = false;
bool g_drawStatusLabel = false;
bool g_drawBoundingBox = false;
bool g_drawBackground = true;
bool g_drawDepth = true;
bool g_drawFrameId = false;
bool g_captureSkeleton = false;
bool g_convertXYZ = false;

//heokyunam capture
bool g_allsave = false;
#define JOINT_SIZE 11
std::vector<nite::Point3f> screen1[JOINT_SIZE], screen2[JOINT_SIZE], merged[JOINT_SIZE];

int g_nXRes = 0, g_nYRes = 0;

int g_mouseX = 0, g_mouseY = 0;

int g_skeletonCount = 0;

// time to hold in pose to exit program. In milliseconds.
const int g_poseTimeoutToExit = 100000;

int main(int argc, char** argv)
{
	openni::Status rc = openni::STATUS_OK;

	SampleViewer sampleViewer("User Viewer");

	rc = sampleViewer.Init(argc, argv);
	if (rc != openni::STATUS_OK)
	{
		return 1;
	}
	sampleViewer.Run();
}

void SampleViewer::glutIdle()
{
	glutPostRedisplay();
}

void SampleViewer::glutDisplay()
{
	SampleViewer::ms_self->Display();
}

void SampleViewer::glutKeyboard(unsigned char key, int x, int y)
{
	SampleViewer::ms_self->OnKey(key, x, y);
}

void SampleViewer::glutMouse(int button, int state, int x, int y)
{
    SampleViewer::ms_self->OnMouse(button, state, x, y);
}

SampleViewer::SampleViewer(const char* strSampleName) : m_poseUser(0), m_poseUser2(0) 
{
	ms_self = this;
	strncpy(m_strSampleName, strSampleName, ONI_MAX_STR);
	m_pUserTracker = new nite::UserTracker;
    m_pUserTracker2 = new nite::UserTracker;
}

SampleViewer::~SampleViewer()
{
	Finalize();

    delete[] m_pDepthTexMap;
    delete[] m_pDepthTexMap2;
    
	ms_self = NULL;
}

void SampleViewer::Finalize()
{
	delete m_pUserTracker;
    delete m_pUserTracker2;
	nite::NiTE::shutdown();
	openni::OpenNI::shutdown();
}

openni::Status SampleViewer::Init(int argc, char **argv)
{
    m_pDepthTexMap = NULL;
    m_pDepthTexMap2 = NULL;
    
	openni::Status rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		printf("Failed to initialize OpenNI\n%s\n", openni::OpenNI::getExtendedError());
		return rc;
	}

	const char* deviceUri = openni::ANY_DEVICE;
    const char* deviceUri2 = openni::ANY_DEVICE;
	for (int i = 1; i < argc-1; ++i)
	{
		if (strcmp(argv[i], "-device") == 0)
		{
			deviceUri = argv[i+1];
			break;
		}
	}
	
	openni::Array<openni::DeviceInfo> deviceInfoList;
    
	openni::OpenNI::enumerateDevices(&deviceInfoList);

    deviceUri = deviceInfoList[2].getUri();
    deviceUri2 = deviceInfoList[5].getUri();
    
// [ dev1 open ]
	rc = m_device.open(deviceUri);
	if (rc != openni::STATUS_OK)
	{
		printf("Failed to open device\n%s\n", openni::OpenNI::getExtendedError());
		return rc;
	}
	printf("%s \n", m_device.getDeviceInfo().getUri());

// [ dev2 open ]
	rc = m_device2.open(deviceUri2);
	if (rc != openni::STATUS_OK)
	{
		printf("Failed to open device\n%s\n", openni::OpenNI::getExtendedError());
		return rc;
	}
	printf("%s \n", m_device2.getDeviceInfo().getUri());

	nite::NiTE::initialize();

    // [ dev1 user tracker create ]
	if (m_pUserTracker->create(&m_device) != nite::STATUS_OK)
	{
		return openni::STATUS_ERROR;
	}
	
// [ dev2 user tracker create ]
	if (m_pUserTracker2->create(&m_device2) != nite::STATUS_OK)
    {
        return openni::STATUS_ERROR;
    }    
    
	return InitOpenGL(argc, argv);

}
openni::Status SampleViewer::Run()	//Does not return
{
	glutMainLoop();

	return openni::STATUS_OK;
}

float Colors[][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 1}};
int colorCount = 3;

#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};
char g_userStatusLabels[MAX_USERS][100] = {{0}};

char g_generalMessage[100] = {0};

GLuint depthTexture;

#define USER_MESSAGE(msg) {\
	sprintf(g_userStatusLabels[user.getId()], "%s", msg);\
	printf("[%08" PRIu64 "] User #%d:\t%s\n", ts, user.getId(), msg);}

void updateUserState(const nite::UserData& user, uint64_t ts)
{
	if (user.isNew())
	{
		USER_MESSAGE("New");
	}
	else if (user.isVisible() && !g_visibleUsers[user.getId()])
		printf("[%08" PRIu64 "] User #%d:\tVisible\n", ts, user.getId());
	else if (!user.isVisible() && g_visibleUsers[user.getId()])
		printf("[%08" PRIu64 "] User #%d:\tOut of Scene\n", ts, user.getId());
	else if (user.isLost())
	{
		USER_MESSAGE("Lost");
	}
	g_visibleUsers[user.getId()] = user.isVisible();


	if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
	{
		switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
		{
		case nite::SKELETON_NONE:
			USER_MESSAGE("Stopped tracking.")
			break;
		case nite::SKELETON_CALIBRATING:
			USER_MESSAGE("Calibrating...")
			break;
		case nite::SKELETON_TRACKED:
			USER_MESSAGE("Tracking!")
			break;
		case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
		case nite::SKELETON_CALIBRATION_ERROR_HANDS:
		case nite::SKELETON_CALIBRATION_ERROR_LEGS:
		case nite::SKELETON_CALIBRATION_ERROR_HEAD:
		case nite::SKELETON_CALIBRATION_ERROR_TORSO:
			USER_MESSAGE("Calibration Failed... :-|")
			break;
		}
	}
}

#ifndef USE_GLES
void glPrintString(void *font, const char *str)
{
	int i,l = (int)strlen(str);

	for(i=0; i<l; i++)
	{   
		glutBitmapCharacter(font,*str++);
	}   
}
#endif

void DrawStatusLabel(nite::UserTracker* pUserTracker, const nite::UserData& user)
{
	int color = user.getId() % colorCount;
	glColor3f(1.0f - Colors[color][0], 1.0f - Colors[color][1], 1.0f - Colors[color][2]);

	float x,y;
	pUserTracker->convertJointCoordinatesToDepth(user.getCenterOfMass().x, user.getCenterOfMass().y, user.getCenterOfMass().z, &x, &y);
	x *= GL_WIN_SIZE_X/(float)g_nXRes;
	y *= GL_WIN_SIZE_Y/(float)g_nYRes;
	char *msg = g_userStatusLabels[user.getId()];
	glRasterPos2i(x-((strlen(msg)/2)*8),y);
	glPrintString(GLUT_BITMAP_HELVETICA_18, msg);
}

void DrawFrameId(int frameId)
{
	char buffer[80] = "";
	sprintf(buffer, "%d", frameId);
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2i(20, 20);
	glPrintString(GLUT_BITMAP_HELVETICA_18, buffer);
}

void DrawCenterOfMass(nite::UserTracker* pUserTracker, const nite::UserData& user)
{
	glColor3f(1.0f, 1.0f, 1.0f);

	float coordinates[3] = {0};

	pUserTracker->convertJointCoordinatesToDepth(user.getCenterOfMass().x, user.getCenterOfMass().y, user.getCenterOfMass().z, &coordinates[0], &coordinates[1]);

	coordinates[0] *= GL_WIN_SIZE_X/(float)g_nXRes;
	coordinates[1] *= GL_WIN_SIZE_Y/(float)g_nYRes;
	glPointSize(8);
	glVertexPointer(3, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_POINTS, 0, 1);

}
void DrawBoundingBox(const nite::UserData& user)
{
	glColor3f(1.0f, 1.0f, 1.0f);

	float coordinates[] =
	{
		user.getBoundingBox().max.x, user.getBoundingBox().max.y, 0,
		user.getBoundingBox().max.x, user.getBoundingBox().min.y, 0,
		user.getBoundingBox().min.x, user.getBoundingBox().min.y, 0,
		user.getBoundingBox().min.x, user.getBoundingBox().max.y, 0,
	};
	coordinates[0]  *= GL_WIN_SIZE_X/(float)g_nXRes;
	coordinates[1]  *= GL_WIN_SIZE_Y/(float)g_nYRes;
	coordinates[3]  *= GL_WIN_SIZE_X/(float)g_nXRes;
	coordinates[4]  *= GL_WIN_SIZE_Y/(float)g_nYRes;
	coordinates[6]  *= GL_WIN_SIZE_X/(float)g_nXRes;
	coordinates[7]  *= GL_WIN_SIZE_Y/(float)g_nYRes;
	coordinates[9]  *= GL_WIN_SIZE_X/(float)g_nXRes;
	coordinates[10] *= GL_WIN_SIZE_Y/(float)g_nYRes;

	glPointSize(2);
	glVertexPointer(3, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

}


// [ TODO : set device's skeleton position ]
void DrawLimb(nite::UserTracker* pUserTracker, const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2, int color)
{
	float coordinates[6] = {0};
    //Critical part
	pUserTracker->convertJointCoordinatesToDepth(joint1.getPosition().x, joint1.getPosition().y, joint1.getPosition().z, &coordinates[0], &coordinates[1]);
	pUserTracker->convertJointCoordinatesToDepth(joint2.getPosition().x, joint2.getPosition().y, joint2.getPosition().z, &coordinates[3], &coordinates[4]);
    
	coordinates[0] *= GL_WIN_SIZE_X/2/(float)g_nXRes;
	coordinates[1] *= GL_WIN_SIZE_Y/2/(float)g_nYRes;
	coordinates[3] *= GL_WIN_SIZE_X/2/(float)g_nXRes;
	coordinates[4] *= GL_WIN_SIZE_Y/2/(float)g_nYRes;

	if (joint1.getPositionConfidence() == 1.0f && joint2.getPositionConfidence() == 1.0f)
	{
		glColor3f(1.0f - Colors[color][0], 1.0f - Colors[color][1], 1.0f - Colors[color][2]);
	}
	else if (joint1.getPositionConfidence() < 0.5f || joint2.getPositionConfidence() < 0.5f)
	{
		return;
	}
	else
	{
		glColor3f(.5, .5, .5);
	}
	glPointSize(2);
	glVertexPointer(3, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_LINES, 0, 2);

	glPointSize(10);
	if (joint1.getPositionConfidence() == 1)
	{
		glColor3f(1.0f - Colors[color][0], 1.0f - Colors[color][1], 1.0f - Colors[color][2]);
	}
	else
	{
		glColor3f(.5, .5, .5);
	}
	glVertexPointer(3, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_POINTS, 0, 1);

	if (joint2.getPositionConfidence() == 1)
	{
		glColor3f(1.0f - Colors[color][0], 1.0f - Colors[color][1], 1.0f - Colors[color][2]);
	}
	else
	{
		glColor3f(.5, .5, .5);
	}
	glVertexPointer(3, GL_FLOAT, 0, coordinates+3);
	glDrawArrays(GL_POINTS, 0, 1);
}
// [ TODO : end ]

void DrawSkeleton(nite::UserTracker* pUserTracker, const nite::UserData& userData)
{
	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_HEAD), userData.getSkeleton().getJoint(nite::JOINT_NECK), userData.getId() % colorCount);

	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW), userData.getId() % colorCount);
	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW), userData.getSkeleton().getJoint(nite::JOINT_LEFT_HAND), userData.getId() % colorCount);

	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW), userData.getId() % colorCount);
	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND), userData.getId() % colorCount);

	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getId() % colorCount);

	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getId() % colorCount);
	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getId() % colorCount);

	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getId() % colorCount);
	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP), userData.getId() % colorCount);

	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP), userData.getId() % colorCount);


	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE), userData.getId() % colorCount);
	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE), userData.getSkeleton().getJoint(nite::JOINT_LEFT_FOOT), userData.getId() % colorCount);

	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE), userData.getId() % colorCount);
	DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT), userData.getId() % colorCount);
}

//heokyunam vector
nite::Point3f findLength(nite::Point3f& a, nite::Point3f& b) {
    float dif[3];
    dif[0] = a.x - b.x; dif[1] = a.y - b.y; dif[2] = a.z - b.z;
    float length = sqrt(dif[0] * dif[0] + dif[1] * dif[1] + dif[2] * dif[2]);
    
    return nite::Point3f(dif[0]/length, dif[1]/length, dif[2]/length);
}

void SampleViewer::Display(){
    openni::VideoFrameRef depthFrame;
    nite::UserTrackerFrameRef userTrackerFrame;
    
    openni::VideoFrameRef depthFrame2;
    nite::UserTrackerFrameRef userTrackerFrame2;

// [ init userTrackerFrame ]
    nite::Status rc = m_pUserTracker->readFrame(&userTrackerFrame);
    
    if ( rc != nite::STATUS_OK ){
        printf("GetNextData Failed\n");
        return;
    }

    rc = m_pUserTracker2->readFrame(&userTrackerFrame2);

    if ( rc != nite::STATUS_OK ){
        printf("GetNextData Failed\n");
        return;
    }
// [ end ]
    nite::Plane c1_floor = userTrackerFrame.getFloor();
    std::cout << c1_floor.point.x << "," << c1_floor.point.y << ","  << c1_floor.point.z << std::endl;
    // init DepthFrame Texture
    if(g_drawDepth){
        // get depthFrame from dev1
        depthFrame = userTrackerFrame.getDepthFrame();

        if( m_pDepthTexMap == NULL ){
            m_nTexMapX = MIN_CHUNKS_SIZE(depthFrame.getVideoMode().getResolutionX(), TEXTURE_SIZE);
            m_nTexMapY = MIN_CHUNKS_SIZE(depthFrame.getVideoMode().getResolutionY(), TEXTURE_SIZE);
            m_pDepthTexMap = new openni::RGB888Pixel[m_nTexMapX * m_nTexMapY];
        }
        
        // get depthFrame from dev2
        depthFrame2 = userTrackerFrame2.getDepthFrame();
                
        if( m_pDepthTexMap2 == NULL ){
            m_nTexMapX2 = MIN_CHUNKS_SIZE(depthFrame2.getVideoMode().getResolutionX(), TEXTURE_SIZE);
            m_nTexMapY2 = MIN_CHUNKS_SIZE(depthFrame2.getVideoMode().getResolutionY(), TEXTURE_SIZE);
            m_pDepthTexMap2 = new openni::RGB888Pixel[m_nTexMapX2 * m_nTexMapY2];
        }
        //calc depth histogram
        if ( depthFrame.isValid() && g_drawDepth ){
            calculateHistogram(m_pDepthHist, MAX_DEPTH, depthFrame);
        }
        
        if ( depthFrame2.isValid() && g_drawDepth ){
            calculateHistogram(m_pDepthHist2, MAX_DEPTH, depthFrame2);
        }
        
        memset(m_pDepthTexMap, 0, m_nTexMapX*m_nTexMapY*sizeof(openni::RGB888Pixel));
        memset(m_pDepthTexMap2, 0, m_nTexMapX2*m_nTexMapY2*sizeof(openni::RGB888Pixel));
        
        const nite::UserMap& userLabels = userTrackerFrame.getUserMap();
        const nite::UserMap& userLabels2 = userTrackerFrame2.getUserMap();

        float factor[3] = {1, 1, 1};
        // draw dev1's Depth Frame
        if ( depthFrame.isValid() && g_drawDepth )
        {
            const nite::UserId* pLabels = userLabels.getPixels();
            
            const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*) depthFrame.getData();
            const openni::DepthPixel* test = (const openni::DepthPixel*) depthFrame.getData();
            openni::RGB888Pixel* pTexRow = m_pDepthTexMap + depthFrame.getCropOriginY() * m_nTexMapX;
            int rowSize = depthFrame.getStrideInBytes() / sizeof(openni::DepthPixel);
            for (int y = 0; y < depthFrame.getHeight(); ++y)
            {
                const openni::DepthPixel* pDepth = pDepthRow;
                openni::RGB888Pixel* pTex = pTexRow + depthFrame.getCropOriginX();
                for (int x = 0; x < depthFrame.getWidth(); ++x, ++pDepth, ++pTex, ++pLabels)
                {
                    if (*pDepth != 0)
                    {
                        if (*pLabels == 0)
                        {
                            if (!g_drawBackground)
                            {
                                factor[0] = factor[1] = factor[2] = 0;

                            }
                            else
                            {
                                factor[0] = Colors[colorCount][0];
                                factor[1] = Colors[colorCount][1];
                                factor[2] = Colors[colorCount][2];
                            }
                        }
                        else
                        {
                            factor[0] = Colors[*pLabels % colorCount][0];
                            factor[1] = Colors[*pLabels % colorCount][1];
                            factor[2] = Colors[*pLabels % colorCount][2];
                        }
    //					// Add debug lines - every 10cm
    // 					else if ((*pDepth / 10) % 10 == 0)
    // 					{
    // 						factor[0] = factor[2] = 0;
    // 					}

                        int nHistValue = m_pDepthHist[*pDepth];
                        pTex->r = nHistValue*factor[0];
                        pTex->g = nHistValue*factor[1];
                        pTex->b = nHistValue*factor[2];
                        
                        factor[0] = factor[1] = factor[2] = 1;

                    }
                }
                pDepthRow += rowSize;
                pTexRow += m_nTexMapX;
            }
        }
        
        // draw dev2's Depth Frame
        if ( depthFrame2.isValid() && g_drawDepth )
        {
            const nite::UserId* pLabels = userLabels2.getPixels();
            
            const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame2.getData();
            openni::RGB888Pixel* pTexRow = m_pDepthTexMap2 + depthFrame2.getCropOriginY() * m_nTexMapX2;
            int rowSize = depthFrame2.getStrideInBytes() / sizeof(openni::DepthPixel);
            for (int y = 0; y < depthFrame2.getHeight(); ++y)
            {
                const openni::DepthPixel* pDepth = pDepthRow;
                openni::RGB888Pixel* pTex = pTexRow + depthFrame2.getCropOriginX();
                for (int x = 0; x < depthFrame2.getWidth(); ++x, ++pDepth, ++pTex, ++pLabels)
                {
                    if (*pDepth != 0)
                    {
                        if (*pLabels == 0)
                        {
                            if (!g_drawBackground)
                            {
                                factor[0] = factor[1] = factor[2] = 0;

                            }
                            else
                            {
                                factor[0] = Colors[colorCount][0];
                                factor[1] = Colors[colorCount][1];
                                factor[2] = Colors[colorCount][2];
                            }
                        }
                        else
                        {
                            factor[0] = Colors[*pLabels % colorCount][0];
                            factor[1] = Colors[*pLabels % colorCount][1];
                            factor[2] = Colors[*pLabels % colorCount][2];
                        }
    //					// Add debug lines - every 10cm
    // 					else if ((*pDepth / 10) % 10 == 0)
    // 					{
    // 						factor[0] = factor[2] = 0;
    // 					}

                        int nHistValue = m_pDepthHist2[*pDepth];
                        pTex->r = nHistValue*factor[0];
                        pTex->g = nHistValue*factor[1];
                        pTex->b = nHistValue*factor[2];

                        factor[0] = factor[1] = factor[2] = 1;
                    }
                }
                pDepthRow += rowSize;
                pTexRow += m_nTexMapX2;
            }
        }
    }
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0, GL_WIN_SIZE_X, GL_WIN_SIZE_Y, 0, -1.0, 1.0 );
    
    //generate gl texture
    GLuint tex[2];
    glGenTextures(2, tex);
    
    if(g_drawDepth){
        //create dev1's texture
        glBindTexture(GL_TEXTURE_2D, tex[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTexMapX, m_nTexMapY, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pDepthTexMap);
        
        //create dev2's texture
        glBindTexture(GL_TEXTURE_2D, tex[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTexMapX2, m_nTexMapY2, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pDepthTexMap2);
        
        g_nXRes = depthFrame.getVideoMode().getResolutionX();
        g_nYRes = depthFrame.getVideoMode().getResolutionY();
    }
    else{
        std::cout << "Depth or RGBFrame flag not set!" << std::endl;
        return;
    }
    
    //draw textures
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    
    glBegin(GL_QUADS);
	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	// upper right
	glTexCoord2f((float)g_nXRes/(float)m_nTexMapX, 0);
	glVertex2f(GL_WIN_SIZE_X/2, 0);
	// bottom right
	glTexCoord2f((float)g_nXRes/(float)m_nTexMapX, (float)g_nYRes/(float)m_nTexMapY);
	glVertex2f(GL_WIN_SIZE_X/2, GL_WIN_SIZE_Y/2);
	// bottom left
	glTexCoord2f(0, (float)g_nYRes/(float)m_nTexMapY);
	glVertex2f(0, GL_WIN_SIZE_Y/2);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    
    glBegin(GL_QUADS);
	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(GL_WIN_SIZE_X/2, 0);
	// upper right
	glTexCoord2f((float)g_nXRes/(float)m_nTexMapX2, 0);
	glVertex2f(GL_WIN_SIZE_X, 0);
	// bottom right
	glTexCoord2f((float)g_nXRes/(float)m_nTexMapX2, (float)g_nYRes/(float)m_nTexMapY2);
	glVertex2f(GL_WIN_SIZE_X, GL_WIN_SIZE_Y/2);
	// bottom left
	glTexCoord2f(0, (float)g_nYRes/(float)m_nTexMapY2);
	glVertex2f(GL_WIN_SIZE_X/2, GL_WIN_SIZE_Y/2);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    
    glDeleteTextures(2, tex);
    
    sk.init(userTrackerFrame, 0, 0);
    sk2.init(userTrackerFrame2, 1, 0);//made by heokyunam
    
// [ tracking skeleton on dev1's user ]
	const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
    nite::Skeleton skeleton_one;
    
	for (int i = 0; i < users.getSize(); ++i)
	{
		const nite::UserData& user = users[i];

		updateUserState(user, userTrackerFrame.getTimestamp());
		if (user.isNew())
		{
			m_pUserTracker->startSkeletonTracking(user.getId());
			m_pUserTracker->startPoseDetection(user.getId(), nite::POSE_CROSSED_HANDS);
		}
		else if (!user.isLost())
		{
			if (g_drawStatusLabel)
			{
				DrawStatusLabel(m_pUserTracker, user);
			}
			if (g_drawCenterOfMass)
			{
				DrawCenterOfMass(m_pUserTracker, user);
			}
			if (g_drawBoundingBox)
			{
				DrawBoundingBox(user);
			}

			if (users[i].getSkeleton().getState() == nite::SKELETON_TRACKED && g_drawSkeleton)
			{
                sk.DrawSkeleton(m_pUserTracker, user);
				//DrawSkeleton(m_pUserTracker, user);
                
                skeleton_one = user.getSkeleton();
			}
		}

		if (m_poseUser == 0 || m_poseUser == user.getId())
		{
			const nite::PoseData& pose = user.getPose(nite::POSE_CROSSED_HANDS);

			if (pose.isEntered())
			{
				// Start timer
				sprintf(g_generalMessage, "In exit pose. Keep it for %d second%s to exit\n", g_poseTimeoutToExit/1000, g_poseTimeoutToExit/1000 == 1 ? "" : "s");
				printf("Counting down %d second to exit\n", g_poseTimeoutToExit/1000);
				m_poseUser = user.getId();
				m_poseTime = userTrackerFrame.getTimestamp();
			}
			else if (pose.isExited())
			{
				memset(g_generalMessage, 0, sizeof(g_generalMessage));
				printf("Count-down interrupted\n");
				m_poseTime = 0;
				m_poseUser = 0;
			}
			else if (pose.isHeld())
			{
				// tick
				if (userTrackerFrame.getTimestamp() - m_poseTime > g_poseTimeoutToExit * 1000)
				{
					printf("Count down complete. Exit...\n");
					Finalize();
					exit(2);
				}
			}
		}
	}
// [ end ]

// [ tracking skeleton on dev2's user ]
	const nite::Array<nite::UserData>& users2 = userTrackerFrame2.getUsers();
    nite::Skeleton skeleton_two;
    
	for (int i = 0; i < users2.getSize(); ++i)
	{
		const nite::UserData& user = users2[i];

		updateUserState(user, userTrackerFrame2.getTimestamp());
		if (user.isNew())
		{
			m_pUserTracker2->startSkeletonTracking(user.getId());
			m_pUserTracker2->startPoseDetection(user.getId(), nite::POSE_CROSSED_HANDS);
		}
		else if (!user.isLost())
		{
			if (g_drawStatusLabel)
			{
				DrawStatusLabel(m_pUserTracker2, user);
			}
			if (g_drawCenterOfMass)
			{
				DrawCenterOfMass(m_pUserTracker2, user);
			}
			if (g_drawBoundingBox)
			{
				DrawBoundingBox(user);
			}

			if (users2[i].getSkeleton().getState() == nite::SKELETON_TRACKED && g_drawSkeleton)
			{
				//DrawSkeleton(m_pUserTracker2, user);
                sk2.DrawSkeleton(m_pUserTracker2, user);//made by heokyunam
                
                skeleton_two = user.getSkeleton();
			}
		}
        
        
		if (m_poseUser2 == 0 || m_poseUser2 == user.getId())
		{
			const nite::PoseData& pose = user.getPose(nite::POSE_CROSSED_HANDS);

			if (pose.isEntered())
			{
				// Start timer
				sprintf(g_generalMessage, "In exit pose. Keep it for %d second%s to exit\n", g_poseTimeoutToExit/1000, g_poseTimeoutToExit/1000 == 1 ? "" : "s");
				printf("Counting down %d second to exit\n", g_poseTimeoutToExit/1000);
				m_poseUser2 = user.getId();
				m_poseTime2 = userTrackerFrame2.getTimestamp();
			}
			else if (pose.isExited())
			{
				memset(g_generalMessage, 0, sizeof(g_generalMessage));
				printf("Count-down interrupted\n");
				m_poseTime2 = 0;
				m_poseUser2 = 0;
			}
			else if (pose.isHeld())
			{
				// tick
				if (userTrackerFrame2.getTimestamp() - m_poseTime2 > g_poseTimeoutToExit * 1000)
				{
					printf("Count down complete. Exit...\n");
					Finalize();
					exit(2);
				}
			}
		}
	}
// [ end ]


    bool isConfident = true;
    //heokyunam capture
    for(int screen = 1; screen < 3; screen++) {
        nite::Skeleton& sk = (screen == 1)? skeleton_one : skeleton_two;
        for(int i = 0; i < JOINT_SIZE; i++) {
            bool jointConfidence = sk.getJoint((nite::JointType)i).getPositionConfidence() == 1.0;
            
            isConfident = isConfident && jointConfidence;//if any joint not confident, every joint can't be used
        }
    }
    
    cv::FileStorage fs;
    cv::Mat R, T;
    cv::Mat tempJoint(3, 1, CV_64F);
    
    fs.open("extrinsics.xml",cv::FileStorage::READ);
    if(fs.isOpened()){
        fs["R"] >> R;
        fs["T"] >> T;
    }
    
// [ capture skeleton ]
    if( isConfident )//heokyunam capture
    {
        nite::Point3f merge;
        for(int i = 0; i < JOINT_SIZE; i++) {
            const nite::Point3f& joint1 = skeleton_one.getJoint((nite::JointType)i).getPosition();
            //std::cout << "[HEOKYUNAM CAPTURE TEST] capture " << joint1.x << " " << joint1.y << " " << joint1.z << std::endl;
            screen1[i].push_back(nite::Point3f(joint1.x, joint1.y, joint1.z));
            
            const nite::Point3f& joint2 = skeleton_two.getJoint((nite::JointType)i).getPosition();
            //std::cout << "[HEOKYUNAM CAPTURE TEST] capture " << joint2.x << " " << joint2.y << " " << joint2.z << std::endl;
            nite::Point3f temp = nite::Point3f(joint2.x, joint2.y, joint2.z);
            screen2[i].push_back(temp);//why bad alloc
            tempJoint.at<double>(0,0) = joint1.x;
            tempJoint.at<double>(1,0) = joint1.y;
            tempJoint.at<double>(2,0) = joint1.z;
            tempJoint = (R * tempJoint) + (10 * T);
            merge = nite::Point3f((tempJoint.at<double>(0,0) + joint2.x)/2, (tempJoint.at<double>(0,1) + joint2.y)/2, (tempJoint.at<double>(0,2) + joint2.z)/2);
            merged[i].push_back(merge);
        }
    }
    
    //if you press the keyboard 'a', it will be saved
    if(g_allsave) {//heokyunam capture
        
        std::string fileDir = "data/skeleton/Up";
        std::string filenames[JOINT_SIZE] = {"JOINT_HEAD", "JOINT_NECK", 
            "JOINT_LEFT_SHOULDER", "JOINT_RIGHT_SHOULDER",
            "JOINT_LEFT_ELBOW", "JOINT_RIGHT_ELBOW",
            "JOINT_LEFT_HAND", "JOINT_RIGHT_HAND",
            "JOINT_TORSO", "JOINT_LEFT_HIP", "JOINT_RIGHT_HIP"
        };
        
        
        for(int i = 0; i < JOINT_SIZE; i++) {
            //making filename
            std::stringstream strStream;
            //fileDir = 'data/skeleton'
            strStream << fileDir.c_str() << "/1/" << filenames[i] << ".csv";
            std::ofstream finFile(strStream.str().c_str());
            strStream.str("");
            
            strStream << fileDir.c_str() << "/2/" << filenames[i] << ".csv";
            std::ofstream finFile2(strStream.str().c_str());
            strStream.str("");
            
            strStream << fileDir.c_str() << "/merged/" << filenames[i] << ".csv";
            std::ofstream finFile3(strStream.str().c_str());
            strStream.str("");
            
            
            //data save
            finFile << "x,y,z" << std::endl;
            for(std::vector<nite::Point3f>::iterator p = screen1[i].begin(); p != screen1[i].end(); p++) {
                finFile << p->x << "," << p->y << "," << p->z << std::endl;
            }
            finFile.close();
            
            finFile2 << "x,y,z" << std::endl;
            for(std::vector<nite::Point3f>::iterator p = screen2[i].begin(); p != screen2[i].end(); p++) {
                finFile2 << p->x << "," << p->y << "," << p->z << std::endl;
            }
            finFile2.close();
            
            finFile3 << "x,y,z" << std::endl;
            for(std::vector<nite::Point3f>::iterator p = merged[i].begin(); p != merged[i].end(); p++) {
                finFile3 << p->x << "," << p->y << "," << p->z << std::endl;
            }
            finFile3.close();
        }
        
        //heokyunam vector
        int pair[] = {
            1,0,            8,1,            1,3,            3,5,            5,7,
            1,2,            2,4,            4,6,            8,10,            8,9
        };
        
        std::string pairnames[] = {"NECK_TO_HEAD", "TORSO_TO_NECK", 
      
            "NECK_TO_RIGHTSHOULDER", "RIGHTSHOULDER_TO_RIGHTELBOW", "RIGHTELBOW_TO_RIGHTHAND", 
            "NECK_TO_LEFTSHOULDER", "LEFTSHOULDER_TO_LEFTELBOW", "LEFTELBOW_TO_LEFTHAND", 
            
            "TORSO_TO_RIGHTHIP",
            "TORSO_TO_LEFTHIP"
        };
        
        for(int i = 0; i < 10; i++) {
            //making filename
            std::stringstream strStream;
            //fileDir = 'data/skeleton'
            strStream << fileDir.c_str() << "/1/" << pairnames[i] << ".csv";
            std::ofstream finFile(strStream.str().c_str());
            strStream.str("");
            
            strStream << fileDir.c_str() << "/2/" << pairnames[i] << ".csv";
            std::ofstream finFile2(strStream.str().c_str());
            strStream.str("");
            
            strStream << fileDir.c_str() << "/merged/" << pairnames[i] << ".csv";
            std::ofstream finFile3(strStream.str().c_str());
            strStream.str("");
            
            //data save
            finFile << "x,y,z" << std::endl;
            for(int k = 0; k < screen1[0].size(); k++) {
                nite::Point3f p = findLength(screen1[pair[2 * i]][k], screen1[pair[2 * i + 1]][k]);
                finFile << p.x << "," << p.y << "," << p.z << std::endl;
            }
            finFile.close();
            
            finFile2 << "x,y,z" << std::endl;
            for(int k = 0; k < screen2[0].size(); k++) {
                nite::Point3f p = findLength(screen2[pair[2 * i]][k], screen2[pair[2 * i + 1]][k]);
                finFile2 << p.x << "," << p.y << "," << p.z << std::endl;
            }
            finFile2.close();
            
            finFile3 << "x,y,z" << std::endl;
            for(int k = 0; k < merged[0].size(); k++) {
                nite::Point3f p = findLength(merged[pair[2 * i]][k], merged[pair[2 * i + 1]][k]);
                finFile3 << p.x << "," << p.y << "," << p.z << std::endl;
            }
            finFile3.close();
        }
        
        std::cout << "save all that" << std::endl;
        g_allsave = false;
    }
        /*
        std::stringstream strStream;
        
        std::cout << "start capturing" << std::endl;
        
        strStream << "data/skeleton/1/" << g_skeletonCount << ".csv";
        std::ofstream finFile(strStream.str().c_str());
        strStream.str("");
        
        strStream << "data/skeleton/2/" << g_skeletonCount << ".csv";
        std::ofstream finFile2(strStream.str().c_str());
        strStream.str("");

        finFile << "TYPE(NUM),X,Y,Z" << std::endl;
        for(int i = 0; i < NITE_JOINT_COUNT; i++){
            const nite::SkeletonJoint& joint = skeleton_one.getJoint((nite::JointType)i);
            finFile << i << "," << joint.getPosition().x << "," << joint.getPosition().y << "," << joint.getPosition().z << std::endl;
        }
        finFile.close();
        
        finFile2 << "TYPE(NUM),X,Y,Z" << std::endl;
        for(int i = 0; i < NITE_JOINT_COUNT; i++){
            const nite::SkeletonJoint& joint = skeleton_two.getJoint((nite::JointType)i);
            finFile2 << i << "," << joint.getPosition().x << "," << joint.getPosition().y << "," << joint.getPosition().z << std::endl;
        }
        finFile2.close();
        
        std::cout << "capture end" << std::endl;
        
        //image capture
        cv::Mat rgbMat; cv::Mat rgbMat2;
        cv::String filename;
        const openni::RGB888Pixel* imageBuffer;
        
        std::cout << "image capture" << std::endl;
        imageBuffer = (const openni::RGB888Pixel*) m_pDepthTexMap;
        cv::Mat(m_nTexMapY, m_nTexMapX, CV_8UC3).copyTo(rgbMat);
        memcpy( rgbMat.data, imageBuffer, 3*m_nTexMapX*m_nTexMapY*sizeof(uint8_t) );
        cv::cvtColor(rgbMat, rgbMat2, CV_RGB2BGR);
        strStream << "images/" << "test/" << depthFrame.getTimestamp() << ".bmp";
        filename = strStream.str();
        cv::imwrite(filename.c_str(), rgbMat2);
        std::cout << strStream.str() << std::endl;
        strStream.str("");
        
        
        g_captureSkeleton = false;
        g_skeletonCount++;
    }*/
// [ end ]

// [ convert xy to XY ]
    if ( g_convertXYZ && g_drawDepth )
    {
        // depth frame [ width : 640 / height : 480 ]
        // camera one 
        // [ width : 0 ~ GL_WIN_SIZE_X / 2 ]
        // [ height : 0 ~ GL_WIN_SIZE_Y / 2 ]
        // camera two
        // [ width : GL_WIN_SIZE_X / 2 ~ GL_WIN_SIZE_X ]
        // [ height : GL_WIN_SIZE_Y / 2 ~ GL_WIN_SIZE_Y ]
        const openni::DepthPixel* depthBuffer = (const openni::DepthPixel*) depthFrame.getData();
        const openni::DepthPixel* depthBuffer2 = (const openni::DepthPixel*) depthFrame2.getData();
        int x = g_mouseX;
        int y = g_mouseY;
        int cx, cy;
        float rx, ry;
        
        std::cout << "Mouse X : " << x << " Mouse Y : " << y << std::endl;
        std::cout << "Width : " << GL_WIN_SIZE_X << " Height : " << GL_WIN_SIZE_Y << std::endl;
        
        if ( x < GL_WIN_SIZE_X / 2 && y < GL_WIN_SIZE_Y / 2 )
        {
            cx = (int) ((float) x / (GL_WIN_SIZE_X / 2) * 640);
            cy = (int) ((float) y / (GL_WIN_SIZE_Y / 2) * 480);
            
            std::cout << "CX : " << cx << " CY : " << cy << std::endl;
            std::cout << "Current Position's(Camera One) Depth : " << depthBuffer[cy*640 + cx] << std::endl;
            // [ TODO : convert xy to XYZ ]
            m_pUserTracker->convertDepthCoordinatesToJoint(cx, cy, depthBuffer[cy*640 + cx], &rx, &ry);
            std::cout << "RX : " << rx << " RY : " << ry << std::endl;
        }
        else if ( x > GL_WIN_SIZE_X / 2 && y < GL_WIN_SIZE_Y / 2 )
        {
            cx = (int) ((float) (x - GL_WIN_SIZE_X / 2) / (GL_WIN_SIZE_X / 2) * 640);
            cy = (int) ((float) y / (GL_WIN_SIZE_Y / 2) * 480);
            
            std::cout << "CX : " << cx << " CY : " << cy << std::endl;
            std::cout << "Current Position's(Camera Two) Depth : " << depthBuffer2[cy*640 + cx] << std::endl;
            
            // [ TODO : convert xy to XYZ ]
            m_pUserTracker2->convertDepthCoordinatesToJoint(cx, cy, depthBuffer[cy*640 + cx], &rx, &ry);
            std::cout << "RX : " << rx << " RY : " << ry << std::endl;
        }
        else
        {
            std::cout << "Mouse Coordinates has error" << std::endl;
        }
        /*
        for( int i = 0; i < 640; i++ ){
            for( int j = 0; j < 480; j++ ){
                std::cout << std::hex << depthBuffer[i*j] << " " << std::ends;
            }
            std::cout << std::endl;
        }
        */
        
        g_convertXYZ = false;
    }
// [ end ]

// [ print frameid ]
	if (g_drawFrameId)
	{
		DrawFrameId(userTrackerFrame.getFrameIndex());
        DrawFrameId(userTrackerFrame2.getFrameIndex());
	}
// [ end ]

	if (g_generalMessage[0] != '\0')
	{
		char *msg = g_generalMessage;
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos2i(100, 20);
		glPrintString(GLUT_BITMAP_HELVETICA_18, msg);
	}
    
    glutSwapBuffers();
}


void SampleViewer::OnKey(unsigned char key, int /*x*/, int /*y*/)
{    
	switch (key)
	{
	case 27:
		Finalize();
		exit (1);
	case 's':
		// Draw skeleton?
		g_drawSkeleton = !g_drawSkeleton;
		break;
	case 'l':
		// Draw user status label?
		g_drawStatusLabel = !g_drawStatusLabel;
		break;
	case 'c':
		// Draw center of mass?
		g_drawCenterOfMass = !g_drawCenterOfMass;
		break;
	case 'x':
		// Draw bounding box?
		g_drawBoundingBox = !g_drawBoundingBox;
		break;
	case 'b':
		// Draw background?
		g_drawBackground = !g_drawBackground;
		break;
	case 'd':
		// Draw depth?
		g_drawDepth = true;
		break;
	case 'f':
		// Draw frame ID
		g_drawFrameId = !g_drawFrameId;
		break;
    case 'a':
        g_allsave = true;
        break;
	}

}

void SampleViewer::OnMouse(int button, int state, int x, int y)
{
    //std::cout << "Mouse Pressed : " << button << std::endl;
    //std::cout << "Mouse state : " << state << std::endl;
    switch(button)
    {
        case 0:
            if(state == GLUT_UP)
            {
                g_captureSkeleton = true;
            }
            break;
            
        case 2:
            if(state == GLUT_UP)
            {
                g_mouseX = x;
                g_mouseY = y;
                g_convertXYZ = true;
            }
            break;
    }
}

openni::Status SampleViewer::InitOpenGL(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow (m_strSampleName);
	// 	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

	InitOpenGLHooks();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	return openni::STATUS_OK;

}
void SampleViewer::InitOpenGLHooks()
{
	glutKeyboardFunc(glutKeyboard);
    glutMouseFunc(glutMouse);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
}
