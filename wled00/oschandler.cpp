#include "wled.h"

void handleOscMessage(OSCMessage &msg) {
    char addr[80];
    msg.getAddress(addr);
    DEBUG_PRINT("<-- OSC message received for address ");
    DEBUG_PRINTLN( addr );
}

void handleOscBundle(OSCBundle &bndl) {
    DEBUG_PRINT("<-- OSC bundle received with ");
    DEBUG_PRINT( bndl.size() );
    DEBUG_PRINTLN(" messages");
}
