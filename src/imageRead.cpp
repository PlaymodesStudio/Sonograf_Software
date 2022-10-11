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
    points[0].x = position;
    points[0].y = 0;
    for (int i = 0; i < image.height; i++)
    {
        //BGR& bgr = source.ptr<BGR>(i)[position];
        //vals[i] = (0.299 * bgr.red/255.0f + 0.587 * bgr.green/255.0f + 0.114 * bgr.blue/255.0f);
        Color color = GetImageColor(image, position, i);
        readVals[i] = (0.299 * color.r/255.0f + 0.587 * color.g/255.0f + 0.114 * color.b/255.0f);
        points[i+1].x = position - (readVals[i] * 50); //Graphic scaling
        //TODO: review line draw
        points[i+1].y = ((float)(i+1) / (float)(size+1)) * 720;
    }
    points[size+1].x = position;
    points[size+1].y = 720;


    for(int i = 0; i < size; i++){
        vals[i] = readVals[i*2] + readVals[(i*2) + 1];    
    }
}

void imageRead::draw(int size){
    //Draw sonify line;
    rlSetLineWidth(3);
    DrawLineStrip(points, size+2, WHITE);
}


