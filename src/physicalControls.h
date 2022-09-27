//
//  physicalControls.h
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#ifndef physicalControls_h
#define physicalControls_h

#if __linux__
    #include "i2c.h"
#endif

class physicalControls{
public:
    physicalControls();
    
    void readValues();
    void drawGui();
    
private:
    bool modeToggle;
    bool captureButton;
    bool freezeButton;
    bool scaleButton;
    
    int speedKnob;
    int transposeKnob;
};

#endif /* physicalControls_h */
