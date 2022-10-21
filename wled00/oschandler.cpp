#include "wled.h"
#include <map>

typedef void (*callbackOSC)(OSCMessage &);
std::map<std::string, callbackOSC> oscmap;

void addHandlerForAddress(std::string addr, callbackOSC cb) {
    oscmap.insert( std::make_pair(addr, cb) );
}

void handleOscMessage(OSCMessage &msg) {
    char addr[80];
    msg.getAddress(addr);
    DEBUG_PRINT("<-- OSC message received for address ");
    DEBUG_PRINTLN( addr );

    for (std::pair<std::string, callbackOSC> element : oscmap) {
        // Serial.print(element.first.c_str());
        // Serial.print(": ");
        // Serial.print(element.second);
        // Serial.println();
        msg.dispatch(element.first.c_str(), element.second);
    }    
}

void handleOscBundle(OSCBundle &bndl) {
    DEBUG_PRINT("<-- OSC bundle received with ");
    DEBUG_PRINT( bndl.size() );
    DEBUG_PRINTLN(" messages");
}
