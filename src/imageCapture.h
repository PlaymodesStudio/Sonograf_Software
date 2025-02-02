//
//  imageCapture.h
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 1/8/22.
//

#ifndef imageCapture_h
#define imageCapture_h

#include "raylib.h"
#include "opencv2/opencv.hpp"
#include <future>
#include <string>

class imageCapture{
public:
    imageCapture(int width, int height, std::string calibrationFile);
    
    void setup();
    void update();
    void captureNewFrame(cv::Mat &dispMat, cv::Mat &readMat);

    void getAndProcessImage(cv::Mat &dispMat, cv::Mat &readMat);
    
    bool isFrameNew();
    cv::Mat& getFrame();

    void draw(float x = 0, float y = 0, float w = -1, float h = -1);
    void drawGui();
   
    void setCalibrate(bool c){calibrate = c;};
    
    std::vector<Vector2>& getAnchorPoints(){return anchorPoints;};
    
private:
    std::future<void> future;
    //Setings
    int camWidth;
    int camHeight;

    //Cam
    cv::VideoCapture capture;
    
    //Warping
    std::vector<Vector2> anchorPoints;
    std::vector<cv::Point2f> srcPoints;
    std::vector<cv::Point2f> dstPoints;
    
    //Pixels
    cv::Mat capturedFrame;
    cv::Mat colorFrame;
    cv::Mat flippedImage;
    cv::Mat warpedFrame;
    cv::Mat transformedFrame;
    cv::Mat resizedMat;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat undistortMapX, undistortMapY;

    //Gui Vars
    int anchorDragIdx;
    int anchorSelectIdx;
    bool calibrate;
    
    Vector2 drag;
    Vector2 prevMouse{0, 0};
};

#endif /* imageCapture_h */
