#include "wled.h"

void handleOscMessage(OSCBundle &bndl) {
    char addr[80];

    if( bndl.size() > 0 ) {
        OSCMessage msg = bndl.getOSCMessage(0); 
        msg.getAddress(addr);
        DEBUG_PRINT("<-- OSC message received with address: ");
        DEBUG_PRINTLN( addr );
    }
}
