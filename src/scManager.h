//
//  scManager.h
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 27/9/22.
//

#ifndef scManager_h
#define scManager_h

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#include "imageCapture.h"
#include "imageRead.h"

#define OUTPUT_BUFFER_SIZE 4096

#include <vector>


enum ScaleTypes_{
    ScaleTypes_Microtonal,// = 0, //Default
    ScaleTypes_Major,// = 1,
    ScaleTypes_Minor,// = 2,
    ScaleTypes_Major_pent,// = 3,
    ScaleTypes_Minor_pent,// = 4,
    ScaleTypes_Major_harm,// = 5,
    ScaleTypes_Minor_harm,// = 6,
    ScaleTypes_Hirajoshi,// = 7,
    ScaleTypes_Arabic,// = 8,
    ScaleTypes_Wholetone,// = 9
    ScaleTypes_COUNT
};

class scManager{
public:
    scManager();
    
    bool setup(std::string address, int port);
    void initialize();
    void update();
    void close();
    
    void setScale(int scale, int transpose = 0);
    void sendAmps(float amps[], int size);
    int getScaleSize(int scale){return scales[scale].size();};
private:
    std::unique_ptr<UdpTransmitSocket> sendSocket; ///< sender socket
    char buffer[OUTPUT_BUFFER_SIZE];
    std::vector<int> scales[ScaleTypes_COUNT];
};

#endif /* scManager_h */
