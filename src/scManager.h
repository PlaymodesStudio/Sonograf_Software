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

class scManager{
public:
    scManager();
    
    bool setup(std::string address, int port);
    void initialize();
    void update();
    
    void setScale(int scale);
private:
    std::unique_ptr<UdpTransmitSocket> sendSocket; ///< sender socket
    char buffer[OUTPUT_BUFFER_SIZE];
};

#endif /* scManager_h */
