#include "AsyncOSC.h"
#include "../network/Network.h"
#include <string.h>

AsyncOSC::AsyncOSC(osc_callback_function callback) {
  _callback = callback;
}

bool AsyncOSC::begin(uint16_t port) {
  return initUnicast(port);
}

bool AsyncOSC::initUnicast(uint16_t port) {
  bool success = false;

  if (udp.listen(port)) {
    udp.onPacket(std::bind(&AsyncOSC::parsePacket, this, std::placeholders::_1));
    success = true;
  }
  return success;
}

void AsyncOSC::parsePacket(AsyncUDPPacket _packet) {
  bool error = false;

  OSCMessage inmsg;

  uint8_t *datap = _packet.data();
  inmsg.fill( datap, _packet.length() );
  error = inmsg.hasError();
  
//   if (error && _packet.localPort() == DDP_DEFAULT_PORT) { //DDP packet
//     error = false;
//     protocol = P_DDP;
//   }

  if (!error && (_callback != nullptr) ) {
    _callback( inmsg );
  }
}