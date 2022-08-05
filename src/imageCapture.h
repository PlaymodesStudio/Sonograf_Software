//
//  imageCapture.h
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 1/8/22.
//

#ifndef imageCapture_h
#define imageCapture_h

#include "ofVideoGrabber.h"
#include "ofImage.h"

class imageCapture{
public:
    imageCapture(){}
    
    void setup();
    void update();
    void draw(float x = 0, float y = 0, float w = -1, float h = -1);
    void drawGui();
    ofPixels& getPixels();
    
private:
    ofVideoGrabber videoGrabber;
    std::vector<glm::vec2> anchorPoints;
    int anchorDragIdx = -1;
    int anchorSelectIdx = -1;
    ofImage unwarped;
};

#endif /* imageCapture_h */
