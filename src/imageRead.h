//
//  imageRead.h
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#ifndef imageRead_h
#define imageRead_h

#include "raylib.h"

class imageRead{
public:
    imageRead();
    
    void update(Image image, int size, int position);
    void draw(int size, int mode);
    
    float* getValues(){return vals;};
    
private:
    Vector2 points[722];
    Vector2 points_third[722];
    float vals[720];
    float readVals[720];
};

#endif /* imageRead_h */
