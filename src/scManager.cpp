//
//  scManager.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#include "scManager.h"
#include <iostream>

scManager::scManager(){
    auto map = [](float value, float inputMin, float inputMax, float outputMin, float outputMax) -> float{
        return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
    };
    
    //Fill scales;
    //Mircotonal
    scales[0].resize(360);
    for(int i = 0; i < 360; i++){
        scales[0][i] = map(i, 0, 359, 108, 48);
    }
    
    //Major
    scales[1].resize(60);
    for(int i = 0; i < 60; i++){
        scales[1][i] = 108-i;
    }
    
    //Minor
    scales[2].resize(60);
    for(int i = 0; i < 60; i++){
        scales[2][i] = 108-i;
    }
    
    //Major_Pentatonic
    scales[3].resize(60);
    for(int i = 0; i < 60; i++){
        scales[3][i] = 108-i;
    }
    
    //Minor_Pentatonic
    scales[4].resize(60);
    for(int i = 0; i < 60; i++){
        scales[4][i] = 108-i;
    }
    
    //Major_Harmonic
    scales[5].resize(60);
    for(int i = 0; i < 60; i++){
        scales[5][i] = 108-i;
    }
    
    //Minor_Harmonic
    scales[6].resize(60);
    for(int i = 0; i < 60; i++){
        scales[6][i] = 108-i;
    }
    
    //Hirajoshi
    scales[7].resize(60);
    for(int i = 0; i < 60; i++){
        scales[7][i] = 108-i;
    }
    
    //Arabic
    scales[8].resize(60);
    for(int i = 0; i < 60; i++){
        scales[8][i] = 108-i;
    }
    
    //Wholetone
    scales[9].resize(60);
    for(int i = 0; i < 60; i++){
        scales[9][i] = 108-i;
    }
}

bool scManager::setup(std::string address, int port){
    //Code from ofxOsc/ofxOscSender.cpp
    
    // create socket
    UdpTransmitSocket *socket = nullptr;
    try{
        IpEndpointName name = IpEndpointName(address.c_str(), port);
        if (!name.address){
            std::cout << "bad host? " << address << std::endl;
            return false;
        }
        socket = new UdpTransmitSocket(name);
        sendSocket.reset(socket);
    }
    catch(std::exception &e){
        std::string what = e.what();
        // strip endline as ofLogError already adds one
        if(!what.empty() && what.back() == '\n') {
            what = what.substr(0, what.size()-1);
        }
        std::cout << "couldn't create sender to "
                                    << address << " on port "
                                    << port << ": " << what
                                    << std::endl;
        if(socket != nullptr){
            delete socket;
            socket = nullptr;
        }
        sendSocket.reset();
        return false;
    }
    return true;
    
    
}

void scManager::initialize(){
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/g_new") //Create World
            << 1 << 0 << 0 << osc::EndMessage
        << osc::BeginMessage("/s_new") //Create Synth
            << "KlangTest360_2" << 2000 << 0 << 1 << osc::EndMessage;

    sendSocket->Send(p.Data(), p.Size());

//    int nodeID = 2000;
//    int position = 0;
//    int groupID = 1;
    setScale(0, 0);//Chromatic no transpose
}

void scManager::update(){
    
}

void scManager::close(){
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/n_free") //Free synth
            << 2000 << osc::EndMessage;

    sendSocket->Send(p.Data(), p.Size());
}

void scManager::setScale(int scale, int transpose){
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/n_setn") //Create World
            << 2000 << "freq" << (int)scales[scale].size(); //NodeId / Param Name / Size
    
    for(int i = 0; i < scales[scale].size(); i++){
        p << scales[scale][i] + transpose;
    }
    
    p << osc::EndMessage;
    sendSocket->Send(p.Data(), p.Size());
}


void scManager::sendAmps(float amps[], int size){
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/n_setn") //Create World
            << 2000 << "amp" << 360; //NodeId / Param Name / Size
        
    for(int i = 0; i < 360; i++){
        //TODO: Que es aquest 10?
        if(i < size) p << amps[i];
        else p << 0.0f;
    }
    
    p << osc::EndMessage;
    sendSocket->Send(p.Data(), p.Size());
}
