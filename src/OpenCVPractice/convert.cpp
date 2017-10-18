#include <iostream>
#include <NiTE.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace nite;
using namespace std;
#define DATA_LENGTH 100
#define JOINT_SIZE 11
#define VECTOR_SIZE 10
//loading success
//it read last line twice
bool load_data(vector<cv::Mat>& vec, string filename) {
    char inputString[DATA_LENGTH];
    ifstream stream(filename.c_str());
    if(!stream.is_open()) {
        cout << "load data fail " << filename << " doesn't exists" << endl;
        return false;
    }
    stream.getline(inputString, DATA_LENGTH);
    for(int i = 0; !stream.eof(); i++) {        
        cv::Mat mat(3, 1, CV_64F);
        double temp[3];
        stream.getline(inputString, DATA_LENGTH);
        sscanf(inputString, "%lf,%lf,%lf", &temp[0], &temp[1], &temp[2]);
        //cout << "reading data : " << i << " => " << temp[0] << " " << temp[1] << " " << temp[2] << endl;
        mat.at<double>(0,0) = temp[0];
        mat.at<double>(1,0) = temp[1];
        mat.at<double>(2,0) = temp[2];
        vec.push_back(mat);
    }
    return true;
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
        result.push_back(((source[i] - ((R * target[i]) + (10 * T)))/source[i]) * 100);
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

//convert data for machine learning
void mergeVector(ofstream& merger, ofstream& predict, string fileDir, string answer, int rand_length, bool needHead) {
    
    std::vector<cv::Mat> screen1[VECTOR_SIZE], screen2[VECTOR_SIZE];
    
    std::string filenames[] = {"NECK_TO_HEAD", "TORSO_TO_NECK", 
      
            "NECK_TO_RIGHTSHOULDER", "RIGHTSHOULDER_TO_RIGHTELBOW", "RIGHTELBOW_TO_RIGHTHAND", 
            "NECK_TO_LEFTSHOULDER", "LEFTSHOULDER_TO_LEFTELBOW", "LEFTELBOW_TO_LEFTHAND", 
            
            "TORSO_TO_RIGHTHIP",
            "TORSO_TO_LEFTHIP"
    };
    
    //std::string fileDir = "data/skeleton/stand";
    std::stringstream fnstream;
    for(int i = 0; i < VECTOR_SIZE; i++) {
        fnstream << fileDir.c_str() << "/merged/" << filenames[i] << ".csv";
        load_data(screen1[i], fnstream.str().c_str());
        //cout << "load_data " << i << " " << screen1[0].size() << " " << fnstream.str().c_str() << endl;
        fnstream.str("");/*
        fnstream << fileDir.c_str() << "/2/" << filenames[i] << ".csv";
        load_data(screen2[i], fnstream.str().c_str());
        fnstream.str("");*/
        
    }
    int vec_length = screen1[0].size();
    int* randarr = new int[rand_length];
    srand(time(0));
    for(int i = 0; i < rand_length; i++) {
        randarr[i] = rand() % vec_length;
        for(int j = 0; j < i; j++) {
            if(randarr[i] == randarr[j])
              i--;
        }
    }
    //sorting data
    sort(randarr, randarr + rand_length);
    for(int i = 0; i < rand_length; i++) {
	    cout << randarr[i] << " ";
    }
    cout << endl;
    
    
    std::stringstream head_s;
    if(needHead) {
        for(int j = 0; j < VECTOR_SIZE; j++) {
            head_s << filenames[j] << ".x,";
            head_s << filenames[j] << ".y,";
            head_s << filenames[j] << ".z,";
        }
        head_s << "answer" << std::endl;
        merger << head_s.str();
        predict << head_s.str();
    }
    //vec_length = screen1[0].size()
    int rand_index = 0;
    for(int i = 0; i < vec_length; i++) {
        stringstream row;
        for(int j = 0; j < VECTOR_SIZE; j++) {
            cv::Mat p1, p2;
            p1 = screen1[j][i];
            
            row << p1.at<double>(0,0) << "," << p1.at<double>(1,0) << "," << p1.at<double>(2,0) << ",";
        }
        row << answer.c_str();
	//cout << i << " = " << rand_index << " " << randarr[rand_index] << endl;
        if(rand_index < rand_length && i == randarr[rand_index]) {
            predict << row.str().c_str() << std::endl;
	    rand_index++;
        }
        else {
            merger << row.str().c_str() << std::endl;
        }
        row.str("");
    }
}


void mergeAll() {
    ofstream merger("data/skeleton/data_10181_merge.csv");
    ofstream predict("data/skeleton/predict_10181_merge.csv");
    mergeVector(merger, predict, "data/skeleton/Down", "100", 10, true);
    mergeVector(merger, predict, "data/skeleton/Front", "101", 10, false);
    merger.close();
    
    predict.close();
    
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
{
    //load extrincsics data
    cv::FileStorage fs;
    cv::Mat R, T;
    //average value for result
    fs.open("extrinsics.xml",cv::FileStorage::READ);
    if(fs.isOpened()){
        fs["R"] >> R;
        fs["T"] >> T;
    }
    //printDifference(R, T);
    //mergeVector("stand");
    mergeAll();
    return 0;
}
