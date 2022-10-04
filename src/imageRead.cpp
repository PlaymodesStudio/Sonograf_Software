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

void imageRead::update(cv::Mat mat, int size, int position){
    image.data = (void *)(mat.data);
    texture = LoadTextureFromImage(image);

    cv::Mat resizedMat;
    cv::resize(mat, resizedMat, cv::Size(image.width, size));
    
    //TODO: Inverse image, make adaptative threshold, etc
    struct BGR {
        uchar blue;
        uchar green;
        uchar red;  };
    for (int i = 0; i < size; i++)
    {
        BGR& bgr = resizedMat.ptr<BGR>(i)[position];
        vals[i] = (0.299 * bgr.red/255.0f + 0.587 * bgr.green/255.0f + 0.114 * bgr.blue/255.0f);
        points[i].x = vals[i] * 100; //Graphic scaling
        points[i].y = ((float)i / (float)size) * image.height;
    }
    points[size].x = 0;
    points[size].y = 720;
}

void imageRead::draw(int size){
    DrawTextureEx(texture, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
    //Draw sonify line;
    rlSetLineWidth(3);
    DrawLineStrip(points, size+1, WHITE);
}


