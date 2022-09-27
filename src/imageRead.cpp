//
//  imageRead.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#include "imageRead.h"
#include "rlgl.h"

imageRead::imageRead(){
    
}

void imageRead::setup(int width, int height){
    image.width = width;
    image.height = height;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    image.mipmaps = 1;
//    image_raylib.data = (void *)(out.data);
}

void imageRead::update(cv::Mat mat, int position){
    image.data = (void *)(mat.data);
    texture = LoadTextureFromImage(image);

    for (int i = 0; i < 720; i++)
    {
        vals[i] = GetImageColor(image, position, i).r;
        points[i].x = vals[i];
        points[i].y = i;
    }
}

void imageRead::draw(){
    DrawTextureEx(texture, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
    //Draw sonify line;
    rlSetLineWidth(3);
    DrawLineStrip(points, 720, WHITE);
}


