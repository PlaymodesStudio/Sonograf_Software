//
//  physicalControls.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#include "physicalControls.h"

physicalControls::physicalControls(){
#if __linux__
    I2c * bus;
    bus = new I2c("/dev/i2c-1");
    bus->addressSet(0x08);
#endif
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
//    std::cout << (data[1]<<8|data[0]) << " - " <<(data[3]<<8|data[2]) << " - " << (data[5]<<8|data[4]) << " - " << (data[7]<<8|data[6]) << " - " << (data[9]<<8|data[8]) << " - " << (data[11]<<8|data[10]) << std::endl;
#endif
}

void physicalControls::drawGui(){
    
}
