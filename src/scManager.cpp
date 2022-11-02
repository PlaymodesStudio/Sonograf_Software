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
    
	//Mt [0-12]
	//Chro (0-1-2-3-4-5-6-7-8-9-10-11)
	//Maj (0-2-4-5-7-9-11)
	//Pent (0-2-4-7-9)
	//hira (0-1-5-7-10)
	//whole (0-2-4-6-8-10)

    //Fill scales;
    //Mircotonal
    scales[0].resize(360);
    for(int i = 0; i < 360; i++){
        scales[0][i] = map(i, 0, 359, 120, 48);
    }
    
    //Chromatica
    scales[1].resize(72);
    for(int i = 0; i < scales[1].size(); i++){
        scales[1][i] = 120-i;
    }
    
    //Major
    scales[2].resize(42);
    int major[7] = {0, 2, 4, 5, 7, 9, 11};
    for(int i = 0; i < scales[2].size(); i++){
        scales[2][scales[2].size()-i-1] = 48+major[i%7]+(12*(int)(i/7));
    }
    
    //Pentatonic
    scales[3].resize(30);
    int penta[5] = {0, 2, 4, 7, 9};
    for(int i = 0; i < scales[3].size(); i++){
        scales[3][scales[3].size()-i-1] = 48+penta[i%5]+(12*(int)(i/5));
    }
    
    //Horiji
    scales[4].resize(30);
    int hoi[5] = {0, 1, 5, 7, 10};
    for(int i = 0; i < scales[4].size(); i++){
        scales[4][scales[4].size()-i-1] = 48+hoi[i%5]+(12*(int)(i/5));
    }
    
    //Wholetone
    scales[5].resize(36);
    int whole[6] = {0, 2, 4, 6, 8, 10};
    for(int i = 0; i < scales[5].size(); i++){
        scales[5][scales[5].size()-i-1] = 48+whole[i%6]+(12*(int)(i/6));
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
            << /*"KlangTest360_2"*/ "Sonograf" << 2000 << 0 << 1 << osc::EndMessage;

    sendSocket->Send(p.Data(), p.Size());

//    int nodeID = 2000;
//    int position = 0;
//    int groupID = 1;
    setScale(0, 0);//Microtonal no transpose
		    float zeros[360] = {0};
		    sendAmps(zeros, 360);
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

    float ampsSum = 0;
    for(int i = 0; i < 360; i++){
	ampsSum += amps[i];
    }
    if(ampsSum == 0) ampsSum = 1;

    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/n_setn") //Create World
            << 2000 << "amp" << 360; //NodeId / Param Name / Size
        
    for(int i = 0; i < 360; i++){
        //TODO: Que es aquest 20?
        if(i < size) p << amps[i]/ampsSum;
        else p << 0.0f;
    }
    
    p << osc::EndMessage;
    sendSocket->Send(p.Data(), p.Size());
}
