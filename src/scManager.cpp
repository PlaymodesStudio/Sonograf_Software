//
//  scManager.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#include "scManager.h"

scManager::scManager(){
    
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
    setScale(0);
}

void scManager::update(){
    
}

void scManager::setScale(int scale){
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/n_setn") //Create World
            << 2000 << "freq" << 360; //NodeId / Param Name / Size
    
    auto map = [](float value, float inputMin, float inputMax, float outputMin, float outputMax) -> float{
        return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
    };

    switch(scale){
        case 0:
        {
            for(int i = 0; i < 360; i++){
                p << map(i, 0, 359, 108, 48);
            }
            break;
        }
    }
    
    p << osc::EndMessage;
    sendSocket->Send(p.Data(), p.Size());
}
