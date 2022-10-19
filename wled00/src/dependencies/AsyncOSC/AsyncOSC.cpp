#include "AsyncOSC.h"
#include "../network/Network.h"
#include <string.h>

AsyncOSC::AsyncOSC(osc_callback_function cb) {
  _callback = cb;
}

bool AsyncOSC::begin(uint16_t port) {
  return initUnicast(port);
}

bool AsyncOSC::initUnicast(uint16_t port) {
  bool success = false;

  Serial.println("[OSC] unicast init");

  if (udp.listen(port)) {
    udp.onPacket(std::bind(&AsyncOSC::parsePacket, this, std::placeholders::_1));
    success = true;
  }
  return success;
}

void AsyncOSC::parsePacket(AsyncUDPPacket _packet) {
  bool error = false;

  OSCMessage inmsg;

  Serial.println("[OSC] packet received");

  uint8_t *datap = _packet.data();
  inmsg.fill( datap, _packet.length() );
  error = inmsg.hasError();
  
//   if (error && _packet.localPort() == DDP_DEFAULT_PORT) { //DDP packet
//     error = false;
//     protocol = P_DDP;
//   }

  if( _callback == nullptr ) {
    Serial.print("[OSC] callback not set ");
  } else {
    _callback( inmsg );
  }

  // if (!error && (_callback != nullptr) ) {
  //   _callback( inmsg );
  // } else {
  //   Serial.print("[OSC] error ");
  //   Serial.println( inmsg.getError() );
  // }
}