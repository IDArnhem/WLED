#include "AsyncOSC.h"
#include "../network/Network.h"
#include <string.h>

AsyncOSC::AsyncOSC(osc_bundle_callback_function cbb, osc_message_callback_function cbm) {
  _cbBundle = cbb;
  _cbMessage = cbm;
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

  OSCBundle inbndl;
  OSCMessage inmsg;

  Serial.println("[OSC] packet received");

  uint8_t *datap = _packet.data();

  // parse single OSC message
  inmsg.fill( datap, _packet.length() );
  if( inmsg.getError() ) {
    Serial.print("[OSC] parsing message error: ");
    Serial.println(inmsg.getError());
    error = true;

    // failed to parse a single message
    // try parsing a bundle instead
    {
      // parse OSC bundle
      inbndl.fill( datap, _packet.length() );
      if( !inbndl.getError() ) {
        // recover from previous error
        error = false;
        if( _cbBundle == nullptr ) {
          Serial.print("[OSC] bundle callback not set ");
        } else {
          Serial.print("[OSC] calling bundle callback");
          _cbBundle( inbndl );
        }
      } // if no errors in bundle
    } // parse bundle

  } else {
    if( _cbMessage == nullptr ) {
      Serial.print("[OSC] message callback not set ");
    } else {
      Serial.print("[OSC] calling message callback");
      _cbMessage( inmsg );
    }
  }

  
//   if (error && _packet.localPort() == DDP_DEFAULT_PORT) { //DDP packet
//     error = false;
//     protocol = P_DDP;
//   }


  // if (!error && (_callback != nullptr) ) {
  //   _callback( inmsg );
  // } else {
  //   Serial.print("[OSC] error ");
  //   Serial.println( inmsg.getError() );
  // }
}