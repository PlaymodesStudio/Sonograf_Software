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

class imageCapture{
public:
    imageCapture(int width, int height);
    
    void setup();
    void update();
    void captureNewFrame(Image &dispImage, Image &readImage);

    void getAndProcessImage(Image &dispImage, Image &readImage);
    
    bool isFrameNew();
    cv::Mat& getFrame();

    void draw(float x = 0, float y = 0, float w = -1, float h = -1);
    void drawGui();
    
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

    //Gui Vars
    int anchorDragIdx;
    int anchorSelectIdx;
    bool calibrate;
    
    Vector2 drag;
    Vector2 prevMouse{0, 0};
};

#endif /* imageCapture_h */
