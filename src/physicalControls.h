//
//  physicalControls.h
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#ifndef physicalControls_h
#define physicalControls_h

#include "osc/OscTypes.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <string>
#include <deque>
#include <raylib.h>

class physicalControls : public osc::OscPacketListener{
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

protected:

    virtual void ProcessMessage(const osc::ReceivedMessage &m, const IpEndpointName &remoteEndPoint);
    
private:

    void DrawInfoString(std::string s, int align);

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
    
    int transposeLastValue;

    std::unique_ptr<UdpListeningReceiveSocket, std::function<void(UdpListeningReceiveSocket*)>> listenSocket;
    std::thread listenThread;
    std::mutex mutex;

    int scaleMessageTimer;
    std::string scaleMessage;
    int transposeMessageTimer;
    std::string transposeMessage;
    int modeMessageTimer;
    std::string modeMessage;

    Font myFont;
};

#endif/* physicalControls_h */
