#include "wled.h"

void handleOscMessage(OSCMessage &msg) {
    char addr[80];

    msg.getAddress(addr);
    DEBUG_PRINT("<-- OSC message received with address: ");
    DEBUG_PRINTLN( addr );
}
