#include <NiTE.h>
#include <iostream>
#include <fstream>

#define INPUT_SIZE 3
#define OUTPUT_SIZE 2

/**
 @brief : 문자열을 잘라주는 함수입니다.
 @strOrigin : 자를 데이터
 @strTok : 분기줄 데이터
 @string : 반환형 , 배열로 인자전달
*/
std::string* StringSplit(std::string strTarget, std::string strTok)
{
    int     nCutPos;
    int     nIndex     = 0;
    std::string* strResult = new std::string[10];
 
    while ((nCutPos = strTarget.find_first_of(strTok)) != strTarget.npos)
    {
        if (nCutPos > 0)
        {
            strResult[nIndex++] = strTarget.substr(0, nCutPos);
        }
        strTarget = strTarget.substr(nCutPos+1);
    }
 
    if(strTarget.length() > 0)
    {
        strResult[nIndex++] = strTarget.substr(0, nCutPos);
    }
 
    return strResult;
}

int main(int argc, char** argv){
    std::string file1, file2;
    
	openni::Status rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		printf("Failed to initialize OpenNI\n%s\n", openni::OpenNI::getExtendedError());
		return rc;
	}

	const char* deviceUri = openni::ANY_DEVICE;
    openni::Device		m_device;
    if(argc == 3){
        file1 = argv[1];
        file2 = argv[2];
    }
    else{
        std::cout << "./XYConverter camera_one_data camera_two_data" << std::endl;
        return 0;
    }
    std::ifstream data_one(file1.c_str());
    std::ifstream data_two(file2.c_str());
    
    nite::NiTE::initialize();
    openni::Array<openni::DeviceInfo> deviceInfoList;
    
	openni::OpenNI::enumerateDevices(&deviceInfoList);
        
    deviceUri = deviceInfoList[0].getUri();
    m_device.open(deviceUri);
    nite::UserTracker* userTracker = new nite::UserTracker;
    userTracker->create(&m_device);
    
    std::string data1, data2;
    
    std::ofstream out_file1("camera_convert1.csv");
    std::ofstream out_file2("camera_convert2.csv");
    while(!data_one.eof()){
        float outX, outY;
        
        getline(data_one, data1);
        getline(data_two, data2);
        
        std::string* split_one = StringSplit(data1, ",");
        std::string* split_two = StringSplit(data2, ",");

        userTracker->convertDepthCoordinatesToJoint(atof(split_one[0].c_str()), atof(split_one[1].c_str()), atof(split_one[2].c_str()), &outX, &outY);
        out_file1 << outX << "," << outY << "," << atof(split_one[2].c_str()) << std::endl;
        
        userTracker->convertDepthCoordinatesToJoint(atof(split_two[0].c_str()), atof(split_two[1].c_str()), atof(split_two[2].c_str()), &outX, &outY);
        out_file2 << outX << "," << outY << "," << atof(split_two[2].c_str()) << std::endl;
    }
    
    out_file1.close();
    out_file2.close();
    data_one.close();
    data_two.close();
    
	delete userTracker;
    m_device.close();
    nite::NiTE::shutdown();
	openni::OpenNI::shutdown();

    return 0;
}
