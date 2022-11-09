//
//  physicalControls.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#include "physicalControls.h"
#include "raygui.h"
#include <iostream>
#include <clocale>
#include <cmath>

#define MAXVAL 3300.0f
#define MIDVAL 1650.0f
#define MINTIME 0.1f
#define MIDTIME 4.0f
#define MAXTIME 48.0f

physicalControls::physicalControls(){
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
    speedValue = 4;
    transposeValue = 0;
    scale = 0;
    mode = false;
    capture = false;
    freeze = false;
    captureHold = false;
    freezeHold = false;

    transposeLastValue = 0;

    //Messges init
    scaleMessageTimer = 0;
    transposeMessageTimer = 0;
    modeMessageTimer = 0;


    myFont = LoadFontEx("../assets/FuturaStd-Heavy.otf", 30, 0, 250);

    //Create Socket
    UdpListeningReceiveSocket *socket = nullptr;
    {
	    IpEndpointName name(IpEndpointName::ANY_ADDRESS, 7777);
	    socket = new UdpListeningReceiveSocket(name, this);
	    auto deleter = [](UdpListeningReceiveSocket*socket){
		    // tell the socket to shutdown
		    socket->Break();
		    delete socket;
	    };
	    auto newPtr = std::unique_ptr<UdpListeningReceiveSocket, decltype(deleter)>(socket, deleter);
	    listenSocket = std::move(newPtr);
    }
    listenThread = std::thread([this]{
	    while(listenSocket){
    		listenSocket->Run();
   	}
    });

    listenThread.detach();
}

void physicalControls::readValues(){
	mutex.lock();

	if(captureButton && !captureButton_prevState) capture = true;

#ifndef MODE_BUTTON_FIX
        mode = modeToggle;
#endif
        freeze = freezeButton;

        if(scaleButton && !scaleButton_prevState){
	       	scale = ((scale +1) % 6);
		scaleMessageTimer = 60;
		std::string m = "ESCALA: ";
		switch(scale){
			case 0: m+="MICROTONAL"; break;
			case 1: m+="CROMATICA"; break;
			case 2: m+="MAJOR"; break;
			case 3: m+="PENTATONICA"; break;
			case 4: m+="HIRAJOSHI"; break;
			case 5: m+="TO SENCER"; break;
			default: m+="none"; break;
		}
		scaleMessage = m;
	}

	//TODO: FIRMWARE V2
	if(modeToggle != modeToggle_prevState){
#ifdef MODE_BUTTON_FIX
		if(modeToggle){
			mode = !mode;
			modeMessageTimer = 60;
			if(mode)
				modeMessage = "MODE: DINAMIC";
			else
				modeMessage = "MODE: ESTATIC";
		}
#else
		modeMessageTimer = 60;
		if(modeToggle)		
			modeMessage = "MODE: DINAMIC";
		else
			modeMessage = "MODE: ESTATIC";
#endif
	}


	modeToggle_prevState = modeToggle;
	captureButton_prevState = captureButton;
	scaleButton_prevState = scaleButton;

    //TODO:
    if(speedKnob < MIDVAL){
        speedValue = (MAXTIME - MIDTIME) * pow(((float)speedKnob/MIDVAL)-1, 2) + MIDTIME;
    }else{
        speedValue = -(MIDTIME - MINTIME) * pow(((float)speedKnob/MIDVAL)-1, 2) + MIDTIME;
    }
    transposeValue = (int)((((float)transposeKnob / (MAXVAL))) * 25) - 12;
    
    if(transposeValue != transposeLastValue){
	transposeMessageTimer = 60;
	transposeMessage = "TRANSPOSICIO: " + std::to_string(transposeValue);
    }

    transposeLastValue = transposeValue;

    mutex.unlock();
}

void physicalControls::drawGui(){
	if(scaleMessageTimer > 0){
		DrawInfoString(scaleMessage, 2);
		scaleMessageTimer--;
	}
	if(transposeMessageTimer > 0){
		DrawInfoString(transposeMessage, 1);
		transposeMessageTimer--;
	}
	if(modeMessageTimer > 0){
		DrawInfoString(modeMessage, 0);
		modeMessageTimer--;
	}

}

    void physicalControls::DrawInfoString(std::string s, int align){

	int screenWidth = 1280;
	int screenHeight = 720;
	int fontSize = 30;
	int spacing = 1;
	int textMargin = 5;
	int outerMargin = 10;
	int textLength = MeasureTextEx(myFont, s.c_str(), fontSize, spacing).x;
	int rectPosX = 0;
	switch(align){
		case 0: 
			rectPosX = textMargin + outerMargin;
			break;
		case 1:
			rectPosX = screenWidth/2 - textLength/2 - textMargin;
			break;
		case 2:
			rectPosX = screenWidth - textLength - textMargin - textMargin - outerMargin;
			break;
		default:
			break;
	}
			

	DrawRectangle(rectPosX, 
			screenHeight - fontSize - textMargin - textMargin - outerMargin,
		       	textLength + textMargin + textMargin,
		       	fontSize + textMargin + textMargin,
		       	(Color){ 0, 0, 0, 127} );

	DrawTextEx(myFont,
			s.c_str(),
		       	(Vector2){float(rectPosX + textMargin),
		       		float(screenHeight - textMargin - fontSize - outerMargin)},
			fontSize,
			spacing,
		       	WHITE);
    }

    void physicalControls::ProcessMessage(const osc::ReceivedMessage &m, const IpEndpointName &remoteEndPoint){
	    if(mutex.try_lock()){
		    int i = 0;
		    for(osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin(); arg != m.ArgumentsEnd(); ++arg){
			if(i == 0) scaleButton = arg->AsInt32() < MIDVAL;
		    	else if(i == 1) transposeKnob = MAXVAL - 1 - arg->AsInt32(); 
		    	else if(i == 2) freezeButton = arg->AsInt32() < MIDVAL;
		    	else if(i == 3) speedKnob = MAXVAL - 1 - arg->AsInt32(); 
		    	else if(i == 4) captureButton = arg->AsInt32() < MIDVAL;
			else if(i == 5) modeToggle = arg->AsInt32() < MIDVAL;
			i++;
		    }
		    mutex.unlock();
	    }
    }
