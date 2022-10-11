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
    
    float getSpeed(){return speedValue;};
    int getTranspose(){return transposeValue;};
    int getScale(){return scale;};
    int getMode(){return mode;};
    bool isCapturePressed(){
        if(capture){
            capture = false;
            return true;
        }
        return false;
    }
    
    bool isFreezePressed(){
        if(freeze){
            //freeze = false;
            return true;
        }
        return false;
    }
    
    bool isCaptureHold(){
        return captureHold;
    }
    
    bool isFreezeHold(){
        return freezeHold;
    }
    
private:
    bool modeToggle;
    bool captureButton;
    bool freezeButton;
    bool scaleButton;
    
    int speedKnob;
    int transposeKnob;
    
    //Helpers;
    bool modeToggle_prevState;
    bool captureButton_prevState;
    bool freezeButton_prevState;
    bool scaleButton_prevState;
        
    //Useful Values
    float speedValue;
    int transposeValue;
    int scale;
    bool mode;
    bool capture;
    bool freeze;
    bool captureHold;
    bool freezeHold;
    
#if __linux__
    I2c * bus;
    bool readOrWrite;
#endif
};

#endif /* physicalControls_h */
