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


#define GL_WIN_SIZE_X	1920
#define GL_WIN_SIZE_Y	1080
#define TEXTURE_SIZE	540

#define DEFAULT_DISPLAY_MODE	DISPLAY_MODE_DEPTH

#define MIN_NUM_CHUNKS(data_size, chunk_size)	((((data_size)-1) / (chunk_size) + 1))
#define MIN_CHUNKS_SIZE(data_size, chunk_size)	(MIN_NUM_CHUNKS(data_size, chunk_size) * (chunk_size))

void SampleViewer::Display()
{
// [ get rgb frame from dev1's video stream ]
    openni::VideoFrameRef vfColorFrame;
    
    if( m_vsColorStream.readFrame(&vfColorFrame) != openni::STATUS_OK ){
        printf("GetRGBFrameData Failed\n");
        return;
    }
    
    if (m_pTexMap == NULL)
	{
		// Texture map init
        // read texture map by heokyunam
		// m_nTexMapX = MIN_CHUNKS_SIZE(depthFrame.getVideoMode().getResolutionX(), TEXTURE_SIZE);
		// m_nTexMapY = MIN_CHUNKS_SIZE(depthFrame.getVideoMode().getResolutionY(), TEXTURE_SIZE);
        m_nTexMapX = MIN_CHUNKS_SIZE(vfColorFrame.getVideoMode().getResolutionX(), TEXTURE_SIZE);
        m_nTexMapY = MIN_CHUNKS_SIZE(vfColorFrame.getVideoMode().getResolutionY(), TEXTURE_SIZE);
		m_pTexMap = new openni::RGB888Pixel[m_nTexMapX * m_nTexMapY];
	}
// [ end ]

// [ get frame from dev1's userTracker ]
	nite::UserTrackerFrameRef userTrackerFrame;
	openni::VideoFrameRef depthFrame;
	nite::Status rc = m_pUserTracker->readFrame(&userTrackerFrame);
	if (rc != nite::STATUS_OK)
	{
		printf("GetNextData failed\n");
		return;
	}

	depthFrame = userTrackerFrame.getDepthFrame();
// [ end ]

// [ get rgb frame from dev2's video stream ]
    openni::VideoFrameRef vfColorFrame2;
    
    if( m_vsColorStream2.readFrame(&vfColorFrame2) != openni::STATUS_OK ){
        printf("GetRGBFrameData Failed\n");
        return;
    }
    
    if (m_pTexMap2 == NULL)
	{
		// Texture map init
        // read texture map by heokyunam
		// m_nTexMapX = MIN_CHUNKS_SIZE(depthFrame.getVideoMode().getResolutionX(), TEXTURE_SIZE);
		// m_nTexMapY = MIN_CHUNKS_SIZE(depthFrame.getVideoMode().getResolutionY(), TEXTURE_SIZE);
        m_nTexMapX2 = MIN_CHUNKS_SIZE(vfColorFrame2.getVideoMode().getResolutionX(), TEXTURE_SIZE);
        m_nTexMapY2 = MIN_CHUNKS_SIZE(vfColorFrame2.getVideoMode().getResolutionY(), TEXTURE_SIZE);
		m_pTexMap2 = new openni::RGB888Pixel[m_nTexMapX2 * m_nTexMapY2];
	}
// [ end ]
    
// [ get frame from dev2's userTracker ]
    nite::UserTrackerFrameRef userTrackerFrame2;
    openni::VideoFrameRef depthFrame2;
    rc = m_pUserTracker2->readFrame(&userTrackerFrame2);
    if (rc != nite::STATUS_OK)
    {
        printf("GetNextData failed\n");
        return;
    }
    
    depthFrame2 = userTrackerFrame2.getDepthFrame();
// [ end ]

	const nite::UserMap& userLabels = userTrackerFrame.getUserMap();
    const nite::UserMap& userLabels2 = userTrackerFrame2.getUserMap();
    
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, GL_WIN_SIZE_X, GL_WIN_SIZE_Y, 0, -1.0, 1.0);

// [ if drawDepth is true, calc dev1's depth histogram ]
	if (depthFrame.isValid() && g_drawDepth)
	{
 		calculateHistogram(m_pDepthHist, MAX_DEPTH, depthFrame);
	}
// [ end ] 

// [ if drawDepth is true, calc dev2's depth histogram ]
	if (depthFrame2.isValid() && g_drawDepth)
    {
        calculateHistogram(m_pDepthHist2, MAX_DEPTH, depthFrame2);
    }
// [ end ] 

	memset(m_pTexMap, 0, m_nTexMapX*m_nTexMapY*sizeof(openni::RGB888Pixel));
    memset(m_pTexMap2, 0, m_nTexMapX2*m_nTexMapY2*sizeof(openni::RGB888Pixel));

// [ draw dev1's depth frame to texture ]
	// check if we need to draw depth frame to texture
	if (vfColorFrame.isValid() && g_drawDepth)
	{
		//const nite::UserId* pLabels = userLabels.getPixels();
        
        const openni::RGB888Pixel* pRGBRow = (const openni::RGB888Pixel*)vfColorFrame.getData();
		openni::RGB888Pixel* pTexRow = m_pTexMap + vfColorFrame.getCropOriginY() * m_nTexMapX;
		int rowSize = vfColorFrame.getStrideInBytes() / sizeof(openni::RGB888Pixel);

		for (int y = 0; y < vfColorFrame.getHeight(); ++y)
		{
			const openni::RGB888Pixel* pRGB = pRGBRow;
			openni::RGB888Pixel* pTex = pTexRow + vfColorFrame.getCropOriginX();

			for (int x = 0; x < vfColorFrame.getWidth(); ++x, ++pRGB, ++pTex)
			{
					pTex->r = pRGB->r; 
					pTex->g = pRGB->g;
					pTex->b = pRGB->b;

			}

			pRGBRow += rowSize;
			pTexRow += m_nTexMapX;
		}
	}
// [ end ]

// [ draw dev2's depth frame to texture ]
	// check if we need to draw depth frame to texture
	if (vfColorFrame2.isValid() && g_drawDepth)
	{
		//const nite::UserId* pLabels = userLabels.getPixels();
        
        const openni::RGB888Pixel* pRGBRow = (const openni::RGB888Pixel*)vfColorFrame2.getData();
		openni::RGB888Pixel* pTexRow = m_pTexMap2 + vfColorFrame2.getCropOriginY() * m_nTexMapX2;
		int rowSize = vfColorFrame2.getStrideInBytes() / sizeof(openni::RGB888Pixel);

		for (int y = 0; y < vfColorFrame2.getHeight(); ++y)
		{
			const openni::RGB888Pixel* pRGB = pRGBRow;
			openni::RGB888Pixel* pTex = pTexRow + vfColorFrame2.getCropOriginX();

			for (int x = 0; x < vfColorFrame2.getWidth(); ++x, ++pRGB, ++pTex)
			{
					pTex->r = pRGB->r; 
					pTex->g = pRGB->g;
					pTex->b = pRGB->b;

			}

			pRGBRow += rowSize;
			pTexRow += m_nTexMapX2;
		}
	}
// [ end ]


/*
 * Depth Frame Code
 * 
    float factor2[3] = {1, 1, 1};

// [ draw dev2's depth frame to texture ]
    if(depthFrame2.isValid() && g_drawDepth){
        const nite::UserId* pLabels = userLabels2.getPixels();
        
		const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame2.getData();
		openni::RGB888Pixel* pTexRow = m_pTexMap2 + depthFrame2.getCropOriginY() * m_nTexMapX2;
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
							factor2[0] = factor2[1] = factor2[2] = 0;

						}
						else if ((*pDepth / 10) % 10 == 0)
                        {
                            factor2[0] = factor2[2] = 0;
                        }
						else
						{
							factor2[0] = Colors[colorCount][0];
							factor2[1] = Colors[colorCount][1];
							factor2[2] = Colors[colorCount][2];
						}
					}
					else
					{
						factor2[0] = Colors[*pLabels % colorCount][0];
						factor2[1] = Colors[*pLabels % colorCount][1];
						factor2[2] = Colors[*pLabels % colorCount][2];
					}
//					// Add debug lines - every 10cm
// 					else if ((*pDepth / 10) % 10 == 0)
// 					{
// 						factor[0] = factor[2] = 0;
// 					}

					int nHistValue = m_pDepthHist2[*pDepth];
					pTex->r = nHistValue*factor2[0];
					pTex->g = nHistValue*factor2[1];
					pTex->b = nHistValue*factor2[2];

					factor2[0] = factor2[1] = factor2[2] = 1;
				}
			}
			pDepthRow += rowSize;
			pTexRow += m_nTexMapX2;
		}
    }
// [ end ]
*/

// [ create dev1's gl Texture ]
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTexMapX, m_nTexMapY, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pTexMap);
// [ end ]

// [ display dev1's texture ]
	// Display the OpenGL texture map
	glColor4f(1,1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	g_nXRes = vfColorFrame.getVideoMode().getResolutionX();
	g_nYRes = vfColorFrame.getVideoMode().getResolutionY();

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
	glDisable(GL_TEXTURE_2D);
// [ end ]
    
// [ create dev2's gl Texture ]
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTexMapX2, m_nTexMapY2, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pTexMap2);
// [ end ]

// [ display dev2's texture ]
	// Display the OpenGL texture map
	glColor4f(1,1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	g_nXRes = vfColorFrame2.getVideoMode().getResolutionX();
	g_nYRes = vfColorFrame2.getVideoMode().getResolutionY();
    sk.init(userTrackerFrame, 0, 0);
    sk2.init(userTrackerFrame2, 10, 0);//made by heokyunam

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
// [ end ]

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

// [ capture skeleton ]
    if(g_captureSkeleton)
    {
        std::cout << "start capturing" << std::endl;
        std::ofstream finFile("first.txt");
        std::ofstream finFile2("second.txt");
        
        for(int i = 0; i < NITE_JOINT_COUNT; i++){
            const nite::SkeletonJoint& joint = skeleton_one.getJoint((nite::JointType)i);
            finFile << "( " << joint.getPosition().x << ", " << joint.getPosition().y << ", " << joint.getPosition().z << skeleton_one.getState() << " )" << std::endl;
        }
        finFile.close();
        
        for(int i = 0; i < NITE_JOINT_COUNT; i++){
            const nite::SkeletonJoint& joint = skeleton_two.getJoint((nite::JointType)i);
            finFile2 << "( " << joint.getPosition().x << ", " << joint.getPosition().y << ", " << joint.getPosition().z << skeleton_two.getState()  << " )" << std::endl;
        }
        finFile2.close();
        std::cout << "capture end" << std::endl;
        g_captureSkeleton = false;
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


    const nite::SkeletonJoint& joint = skeleton_one.getJoint(JOINT_HEAD);
	float outX, outY;
    m_pUserTracker->convertJointCoordinatesToDepth(joint.getPosition().x, joint.getPosition().y, joint.getPosition().z, &outX, &outY);
    std::cout << "outX : " << outX << ", outY : " << outY << std::endl;
    
	// Swap the OpenGL display buffers
	glutSwapBuffers();
    
}
