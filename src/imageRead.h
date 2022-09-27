//
//  imageRead.h
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#ifndef imageRead_h
#define imageRead_h

#include "opencv2/opencv.hpp"
#include "raylib.h"

class imageRead{
public:
    imageRead();
    
    void setup(int width, int height);
    void update(cv::Mat mat, int position);
    void draw();
    
    
    
private:
    Image image;
    Texture2D texture;
    
    Vector2 points[720];
    float vals[720];
};

#endif /* imageRead_h */
