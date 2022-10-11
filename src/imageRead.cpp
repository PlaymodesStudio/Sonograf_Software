//
//  imageRead.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#include "imageRead.h"
#include "rlgl.h"
#include "opencv2/opencv.hpp"

imageRead::imageRead(){
    
}

void imageRead::update(Image image, int size, int position){
    cv::Mat source(image.height, image.width, CV_8UC3, image.data);
    cv::Mat resizedMat;
    cv::resize(source, resizedMat, cv::Size(image.width, size));
    
    //TODO: Inverse image, make adaptative threshold, etc
    struct BGR {
        uchar blue;
        uchar green;
        uchar red;  };
    points[0].x = position;
    points[0].y = 0;
    for (int i = 0; i < size; i++)
    {
        BGR& bgr = resizedMat.ptr<BGR>(i)[position];
        vals[i] = (0.299 * bgr.red/255.0f + 0.587 * bgr.green/255.0f + 0.114 * bgr.blue/255.0f);
        points[i+1].x = position + (vals[i] * 100); //Graphic scaling
        //TODO: review line draw
        points[i+1].y = ((float)(i+1) / (float)(size+1)) * image.height;
    }
    points[size+1].x = position;
    points[size+1].y = 720;
}

void imageRead::draw(int size){
    //Draw sonify line;
    rlSetLineWidth(3);
    DrawLineStrip(points, size+2, WHITE);
}


