//
//  imageCapture.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 1/8/22.
//

#include "imageCapture.h"
#include "raymath.h"

imageCapture::imageCapture(int width, int height) : camWidth(width), camHeight(height) {
    anchorDragIdx = -1;
    anchorSelectIdx = -1;
    calibrate = false;

    anchorPoints.resize(4);
    anchorPoints[0] = (Vector2){0.1f * camWidth, 0.1f * camHeight};
    anchorPoints[1] = (Vector2){0.9f * camWidth, 0.1f * camHeight};
    anchorPoints[2] = (Vector2){0.9f * camWidth, 0.9f * camHeight};
    anchorPoints[3] = (Vector2){0.1f * camWidth, 0.9f * camHeight};
    
    srcPoints.resize(4);

    dstPoints.resize(4);
    dstPoints[0] = cv::Point2f(0, 0);
    dstPoints[1] = cv::Point2f(camWidth, 0);
    dstPoints[2] = cv::Point2f(camWidth, camHeight);
    dstPoints[3] = cv::Point2f(0, camHeight);
}

void imageCapture::setup(){
    //--- INITIALIZE VIDEOCAPTURE
    int deviceID = 1;        // 0 = open default camera
    #if __APPLE__
        int apiID = cv::CAP_ANY; // 0 = autodetect default API
    #else
        int apiID = cv::CAP_V4L2; //RPI
    #endif
    capture.open(deviceID, apiID);
    while (!capture.isOpened())
    {
        capture.open(deviceID, apiID);
        std::cout << "ERROR! Unable to open camera\n" << std::endl;
    }
    capture.set(cv::CAP_PROP_FRAME_WIDTH, camWidth);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, camHeight);
}

cv::Mat& imageCapture::update(){
    //TODO: Do this in a seperate thread
    bool captured = capture.read(capturedFrame);
    if(!captured){
        std::cout << "ERROR! Frame not captured" << std::endl;
    }
    
    if(calibrate){
        cv::cvtColor(capturedFrame, colorFrame, cv::COLOR_BGR2RGB);
        cv::flip(colorFrame, transformedFrame, -1); //Fliping arround both axis
    }
    else{
        cv::cvtColor(capturedFrame, colorFrame, cv::COLOR_BGR2RGB);
        cv::flip(colorFrame, flippedImage, -1);
        for(int i = 0; i < anchorPoints.size(); i++) {
            srcPoints[i].x = anchorPoints[i].x;
            srcPoints[i].y = anchorPoints[i].y;
        }
        cv::Mat transform = getPerspectiveTransform(&srcPoints[0], &dstPoints[0]);
        warpPerspective(flippedImage, transformedFrame, transform, transformedFrame.size(), cv::INTER_LINEAR);
    }
    return transformedFrame;
}

void imageCapture::draw(float x, float y, float w, float h){
//    DrawTextureEx(texture, (Vector2){(float)screenWidth, (float)screenHeight}, 180.0f, 1.0f, WHITE);
}

void imageCapture::drawGui(){
    drag = Vector2Subtract(GetMousePosition(), prevMouse);
    prevMouse = GetMousePosition();
    
    //TODO: Remove, just for debug purposes
    if (GetKeyPressed() == ' ')
    {
        calibrate = !calibrate;
    }

    bool someClick = false;
    for (int i = 0; i < 4; i++)
    {
        auto bulletPosition = anchorPoints[i];
        auto mouseToBulletDistance = Vector2Distance(GetMousePosition(), bulletPosition);
        auto bulletSize = 20;
        if (mouseToBulletDistance < bulletSize)
        {
            if (IsMouseButtonPressed(0))
            {
                anchorDragIdx = i;
                anchorSelectIdx = i;
                someClick = true;
            }
        }
    }
    if (IsMouseButtonPressed(0) && !someClick)
    {
        anchorSelectIdx = -1;
    }
    if (IsMouseButtonReleased(0))
        anchorDragIdx = -1;

    if (anchorDragIdx != -1)
    {
        anchorPoints[anchorDragIdx] = Vector2Add(anchorPoints[anchorDragIdx], drag);
    }

    if (anchorSelectIdx != -1)
    {
        if (IsKeyDown(KEY_LEFT))
            anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){-1.0f, 0.0f});
        if (IsKeyDown(KEY_RIGHT))
            anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){1.0f, 0.0f});
        if (IsKeyDown(KEY_UP))
            anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){0.0f, -1.0f});
        if (IsKeyDown(KEY_DOWN))
            anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){0.0f, 1.0f});
    }
    
    // Gui Warping
    if (calibrate)
    {
        for (int i = 0; i < 4; i++)
        {
            Color color = GREEN;
            if (i == anchorSelectIdx)
                color = RED;

            DrawCircle(anchorPoints[i].x, anchorPoints[i].y, 20, color);
        }
    }
}
