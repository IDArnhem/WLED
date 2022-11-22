#include "AsyncOSC.h"
#include "../network/Network.h"
#include <string.h>

AsyncOSC::AsyncOSC() { //osc_bundle_callback_function cbb, osc_message_callback_function cbm) {
  // _cbBundle = cbb;
  // _cbMessage = cbm;
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

void AsyncOSC::addHandlerForAddress(std::string addr, std::function<void(OSCMessage &)> cb) {
    oscmap[addr] = cb;
}

std::function<void(OSCMessage &)> AsyncOSC::getHandlerForAddress(std::string addr)
{
    auto fun = oscmap.find(addr);
    if(fun == oscmap.end()) { return nullptr; }
    return fun->second;
}


void AsyncOSC::handleOscMessage(OSCMessage &msg) {
    char baddr[255];
    msg.getAddress(baddr);
    std::string addr( baddr );

    // callbackOSC handler = getHandlerForAddress( addr );
    // tCallbackOSC_c cb = handler.target<void (*)(OSCMessage &)>();
    //msg.dispatch(baddr, cb);

    std::function<void(OSCMessage &)> handler = getHandlerForAddress( addr );
    if( handler != nullptr) {
      handler( msg );
    } else {
      Serial.print("AsyncOSC no match for >> ");
      Serial.println( baddr );
    }

    //msg.dispatch(baddr, handler);
}

void AsyncOSC::handleOscBundle(OSCBundle &bndl) {
    Serial.print("<-- OSC bundle received with ");
    Serial.print( bndl.size() );
    Serial.println(" messages");
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
        Serial.println("[OSC] calling bundle callback");
        handleOscBundle( inbndl );
      } // if no errors in bundle
    } // parse bundle

  } else {
    Serial.println("[OSC] calling message callback");
    handleOscMessage( inmsg );
  }

  
//   if (error && _packet.localPort() == DDP_DEFAULT_PORT) { //DDP packet
//     error = false;
//     protocol = P_DDP;
//   }


  if(error) {
    Serial.print("[OSC] error ");
    Serial.println( inmsg.getError() );
  }

  // if (!error && (_callback != nullptr) ) {
  //   _callback( inmsg );
  // } else {
  //   Serial.print("[OSC] error ");
  //   Serial.println( inmsg.getError() );
  // }
}