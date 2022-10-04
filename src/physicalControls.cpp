//
//  physicalControls.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#include "physicalControls.h"
#include "raygui.h"

physicalControls::physicalControls(){
#if __linux__
    I2c * bus;
    bus = new I2c("/dev/i2c-1");
    bus->addressSet(0x08);
#endif
    //TODO: Revisar Botons init
    modeToggle = 0;
    captureButton = 0;
    freezeButton = 0;
    scaleButton = 0;
    
    speedKnob = 0;
    transposeKnob = 0;
    
    //Helpers;
    modeToggle_prevState = 0;
    captureButton_prevState = 0;
    freezeButton_prevState = 0;
    scaleButton_prevState = 0;
        
    //Useful Values
    speedValue = 1;
    transposeValue = 0;
    scale = 0;
    mode = false;
    capture = false;
    freeze = false;
    captureHold = false;
    freezeHold = false;
    
    
}

void physicalControls::readValues(){
#if __linux__
    std::vector<uint8_t> data;
    data.resize(12);
    for(int i = 0; i < 6; i++){
        bus->writeByte(0x20+i, 0x01);
        bus->readBlock(0x20+i, 2, data.data() + (i*2));
    }
    modeToggle (data[11]<<8|data[10]) > 100;
    captureButton = (data[9]<<8|data[8]) > 100;
    freezeButton = (data[5]<<8|data[4]) > 100;
    scaleButton = (data[1]<<8|data[0]) > 100;
    
    speedKnob = (data[7]<<8|data[6]);
    transposeKnob = (data[3]<<8|data[2]);
    
    //TODO:
    speedValue = speedKnob / 1024;
    transposeValue = transposeKnob / 1024 * 24 - 12;
#endif
}

void physicalControls::drawGui(){
    //if(GuiWindowBox((Rectangle){0, 0, 120, 200}, "Physical Controls")){
    DrawRectangle(0, 0, 300, 300, (Color){ 0, 0, 0, 127 } );
    speedValue = GuiSlider((Rectangle){100, 10, 100, 20}, "Speed", TextFormat("%2.2f", (float)speedValue), speedValue, 0, 100);
    transposeValue = GuiSlider((Rectangle){100, 40, 100, 20}, "Transpose", TextFormat("%i", (int)transposeValue), transposeValue, -12, 12);
    scale = GuiSlider((Rectangle){100, 70, 100, 20}, "Scale", TextFormat("%i", (int)scale), scale, 0, 9);
    mode = GuiCheckBox((Rectangle){100, 100, 20, 20}, "Mode", mode);
    capture = GuiCheckBox((Rectangle){100, 130, 20, 20}, "Capture", capture);
    freeze = GuiCheckBox((Rectangle){100, 160, 20, 20}, "Freeze", freeze);
    captureHold = GuiCheckBox((Rectangle){100, 190, 20, 20}, "Capture Hold", captureHold);
    freezeHold = GuiCheckBox((Rectangle){100, 220, 20, 20}, "Freeze Hold", freezeHold);
    
    //}
}
