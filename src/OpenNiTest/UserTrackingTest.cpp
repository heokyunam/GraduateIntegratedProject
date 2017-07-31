
/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - Simple Skeleton Sample                               *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#include "NiTE.h"
#include <openni-nite/NiTE.h>
#include "UserTrackingTest.h"
#include <iostream>
#include <fstream>

#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};

#define USER_MESSAGE(msg) \
	{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

void updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
		USER_MESSAGE("New")
	else if (user.isVisible() && !g_visibleUsers[user.getId()])
		USER_MESSAGE("Visible")
	else if (!user.isVisible() && g_visibleUsers[user.getId()])
		USER_MESSAGE("Out of Scene")
	else if (user.isLost())
		USER_MESSAGE("Lost")

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

int main(int argc, char** argv)
{
	nite::UserTracker userTracker;
	nite::UserTracker userTracker2;
    openni::Device* dev1;
    openni::Device* dev2;
    nite::Status niteRc;
    openni::OpenNI::initialize();
	nite::NiTE::initialize();
    dev1 = new openni::Device();
    dev2 = new openni::Device();
    openni::Array<openni::DeviceInfo> infoList;    
    openni::OpenNI::enumerateDevices(&infoList);
    
    dev1->open(infoList[0].getUri());
    dev2->open(infoList[3].getUri());
    

	niteRc = userTracker.create(dev1);
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}
	niteRc = userTracker2.create(dev2);
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker2\n");
		return 3;
	}
	printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");

	nite::UserTrackerFrameRef userTrackerFrame;
	nite::UserTrackerFrameRef userTrackerFrame2;
    nite::Skeleton skel1;
    nite::Skeleton skel2;
    
    bool fileOuted = false;
	
    while (!wasKeyboardHit() && !fileOuted)
	{
        //this is maybe get the depthmap and make skeleton
		niteRc = userTracker.readFrame(&userTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			continue;
		}
		niteRc = userTracker2.readFrame(&userTrackerFrame2);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed2\n");
			continue;
		}

		const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
		const nite::Array<nite::UserData>& users2 = userTrackerFrame2.getUsers();
        
        bool dev1captured = false; bool dev2captured = false;
        
		for (int i = 0; i < users.getSize(); ++i)
		{
			const nite::UserData& user = users[i];
			updateUserState(user,userTrackerFrame.getTimestamp());
			if (user.isNew())
			{
				userTracker.startSkeletonTracking(user.getId());
			}
			else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
                nite::Skeleton skel = user.getSkeleton();
                const nite::SkeletonJoint& head = skel.getJoint(nite::JOINT_HEAD);
                if(head.getPositionConfidence() > .5){
                    skel1 = skel;
                    dev1captured = true;
                }
                else{
                    dev1captured = false;
                }
            }
		}

		for (int i = 0; i < users2.getSize(); ++i)
		{
			const nite::UserData& user = users2[i];
			updateUserState(user,userTrackerFrame2.getTimestamp());
			if (user.isNew())
			{
				userTracker2.startSkeletonTracking(user.getId());
			}
			else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
                nite::Skeleton skel = user.getSkeleton();
                const nite::SkeletonJoint& head = skel.getJoint(nite::JOINT_HEAD);
                if(head.getPositionConfidence() > .5){
                    skel2 = skel;
                    dev2captured = true;
                }
                else{
                    dev2captured = false;
                }
			}
		}
		std::cout << dev1captured << "\t\t" << dev2captured << std::endl;
        if(dev2captured && dev1captured){
            std::cout << "start capturing" << std::endl;
            std::ofstream finFile("first.txt");
            std::ofstream finFile2("second.txt");
            for(int i = 0; i < NITE_JOINT_COUNT; i++){
                const nite::SkeletonJoint& joint = skel1.getJoint((nite::JointType)i);
                finFile << "( " << joint.getPosition().x << ", " << joint.getPosition().y << ", " << joint.getPosition().z << skel1.getState() << " )" << std::endl;
            }
            finFile.close();
            for(int i = 0; i < NITE_JOINT_COUNT; i++){
                const nite::SkeletonJoint& joint = skel2.getJoint((nite::JointType)i);
                finFile2 << "( " << joint.getPosition().x << ", " << joint.getPosition().y << ", " << joint.getPosition().z << skel2.getState()  << " )" << std::endl;
            }
            finFile2.close();
            fileOuted = true;
        }
	}

	nite::NiTE::shutdown();

}
