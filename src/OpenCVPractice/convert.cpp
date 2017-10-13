#include <iostream>
#include <NiTE.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace nite;
using namespace std;
#define DATA_LENGTH 100
#define JOINT_SIZE 11;
//loading success
//it read last line twice
void load_data(vector<cv::Mat>& vec, string filename) {
    char inputString[DATA_LENGTH];
    ifstream stream(filename.c_str());
    stream.getline(inputString, DATA_LENGTH);
    while(!stream.eof()) {
        cv::Mat mat(3, 1, CV_64F);
        double temp[3];
        stream.getline(inputString, DATA_LENGTH);
        sscanf(inputString, "%lf,%lf,%lf", &temp[0], &temp[1], &temp[2]);
        mat.at<double>(0,0) = temp[0];
        mat.at<double>(1,0) = temp[1];
        mat.at<double>(2,0) = temp[2];
        vec.push_back(mat);
    }
}

//original method. it put a difference about converting point and another point
void printDifference(cv::Mat R, cv::Mat T) {
    std::cout << R << std::endl;
    std::cout << T << std::endl;
    vector<cv::Mat> source;
    load_data(source, "data/skeleton/2/JOINT_HEAD.csv");
    vector<cv::Mat> target;
    load_data(target, "data/skeleton/1/JOINT_HEAD.csv");
    vector<cv::Mat> result;
    
    double x = 0, y = 0, z = 0;
    for(int i = 0; i < target.size(); i++) {
        result.push_back(source[i] - ((R * target[i]) + (10 * T)));
        cout << result[i].t() << endl;
        x = (x + result[i].at<double>(0,0));
        y = (y + result[i].at<double>(1,0));
        z = (z + result[i].at<double>(2,0));
    }
    x = x / (result.size() + 1);
    y = y / (result.size() + 1);
    z = z / (result.size() + 1);
    cout << "average" << endl << "x : " << x << " y : " << y << " z : " << z << endl;
}

void mergeVector(std::string answer) {
    
    std::vector<cv::Mat> screen1[10], screen2[10];
    
    std::string filenames[] = {"NECK_TO_HEAD", "TORSO_TO_NECK", 
      
            "NECK_TO_RIGHTSHOULDER", "RIGHTSHOULDER_TO_RIGHTELBOW", "RIGHTELBOW_TO_RIGHTHAND", 
            "NECK_TO_LEFTSHOULDER", "LEFTSHOULDER_TO_LEFTELBOW", "LEFTELBOW_TO_LEFTHAND", 
            
            "TORSO_TO_RIGHTHIP",
            "TORSO_TO_LEFTHIP"
    };
    std::string fileDir = "data/skeleton/stand";
    std::stringstream fnstream;
    for(int i = 0; i < 10; i++) {
        fnstream << fileDir.c_str() << "/1/" << filenames[i] << ".csv";
        load_data(screen1[i], fnstream.str().c_str());
        fnstream.str("");
        fnstream << fileDir.c_str() << "/2/" << filenames[i] << ".csv";
        load_data(screen2[i], fnstream.str().c_str());
        fnstream.str("");
        
    }
    
    fnstream << fileDir.c_str() << "/1/sk.csv";
    std::ofstream skData(fnstream.str().c_str());
    fnstream.str("");
    fnstream << fileDir.c_str() << "/2/sk.csv";
    std::ofstream skData2(fnstream.str().c_str());
    std::stringstream head_stream;
    
    for(int j = 0; j < 10; j++) {
        head_stream << filenames[j] << ".x,";
        head_stream << filenames[j] << ".y,";
        head_stream << filenames[j] << ".z,";
        if(j == 9)
            head_stream << "answer" << std::endl;
    }
    skData << head_stream.str().c_str();
    skData2 << head_stream.str().c_str();    
    
    for(int i = 0; i < screen1[0].size(); i++) {
        for(int j = 0; j < 10; j++) {
            cv::Mat p1, p2;
            p1 = screen1[j][i];
            p2 = screen2[j][i];
            skData << p1.at<double>(0,0) << "," << p1.at<double>(1,0) << "," << p1.at<double>(2,0) << ",";
            skData2 << p2.at<double>(0,0) << "," << p2.at<double>(1,0) << "," << p2.at<double>(2,0) << ",";
        }
        skData << answer.c_str() << std::endl;
        skData2 << answer.c_str() << std::endl;
    }
    
    skData.close();
    skData2.close();
}

//only for matlab
void printForMatlab(cv::Mat R, cv::Mat T) {
    vector<cv::Mat> source;
    load_data(source, "data/skeleton/1/sk.csv");
    vector<cv::Mat> result;
    cout << "[" << endl;
    for(int i = 0; i < source.size()-1; i++) {
        cv::Mat src = source[i];
        //cout << src.at<double>(0,0) << " " << src.at<double>(1,0) << " " << src.at<double>(2,0) << ";" << endl;
        cv::Mat tfp = (R * src) + (10 * T);
        cout << tfp.at<double>(0,0) << " " << tfp.at<double>(1,0) << " " << tfp.at<double>(2,0) << ";" << endl;
        result.push_back(tfp);
    }
    cout << "]" << endl;
}

int main(void)
{/*
    //load extrincsics data
    cv::FileStorage fs;
    cv::Mat R, T;
    //average value for result
    fs.open("extrinsics.xml",cv::FileStorage::READ);
    if(fs.isOpened()){
        fs["R"] >> R;
        fs["T"] >> T;
    }*/
    mergeVector("stand");
    return 0;
}
