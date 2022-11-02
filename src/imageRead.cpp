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
    points_third[0].x = (GetScreenWidth() /3.0f);
    points_third[0].y = 0;
    for (int i = 0; i < size; i++)
    {
	int index = (((float)i / (float)size) * image.height);
	float tempSum = 0;
	int steps = 0;
	for(int j = floor(index); j < floor(index + (image.height/size)); j++){
		Color color = GetImageColor(image, position, j);
		tempSum += (0.299 * color.r/255.0f + 0.587 * color.g/255.0f + 0.114 * color.b/255.0f);
		steps++;
	}
	readVals[i] = tempSum/steps;
	vals[i] = readVals[i];
        points[i+1].x = position - (readVals[i] * 50); //Graphic scaling
        points[i+1].y = ((float)(i+1) / (float)(size+1)) * 720;
        points_third[i+1].x = (GetScreenWidth() /3.0f) - (readVals[i] * 50);
        points_third[i+1].y = points[i+1].y;
    }
    for (int i = size; i < 360; i++){
	vals[i] = 0;
    }
    points[size+1].x = position;
    points[size+1].y = 720;
    points_third[size+1].x = (GetScreenWidth() / 3.0f);
    points_third[size+1].y = 720;
}

void imageRead::draw(int size, int mode){
    //Draw sonify line;
    rlSetLineWidth(3);
    if(mode == 0)
        DrawLineStrip(points, size+2, WHITE);
    else
        DrawLineStrip(points_third, size+2, WHITE);
        
}


