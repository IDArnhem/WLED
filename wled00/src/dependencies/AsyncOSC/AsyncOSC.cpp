#include "AsyncOSC.h"
#include "../network/Network.h"
#include <string.h>

AsyncOSC::AsyncOSC() { //osc_bundle_callback_function cbb, osc_message_callback_function cbm) {
  // _cbBundle = cbb;
  // _cbMessage = cbm;
}

bool AsyncOSC::begin(uint16_t port) {
  Serial.print("[OSC] Listening on port ");
  Serial.print( port );
  Serial.println();
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

    std::function<void(OSCMessage &)> handler = getHandlerForAddress( addr );
    
    if( handler != nullptr) {
      
      handler( msg ); // say your prayers

      // @note we can't call the message dispatch method
      // because we can't convert our C++-style handler to a void(OSCMessage &) C-style function pointer
      // the dispatch method does some address verification that we don't do here and then calls the handler with
      // the message as a parameter, whcih we do.
      //msg.dispatch(baddr, handler);
    } else {
      Serial.print("[OSC] no handler for >> ");
      Serial.println( baddr );
    }
}

void AsyncOSC::handleOscBundle(OSCBundle &bndl) {
  Serial.println("[OSC] BUNDLES NOT SUPPORTED YET");
    // Serial.print("<-- OSC bundle received with ");
    // Serial.print( bndl.size() );
    // Serial.println(" messages");
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

  
  if(error) {
    Serial.print("[OSC] error ");
    Serial.println( inmsg.getError() );
  }
}

void AsyncOSC::broadcast( OSCMessage &msg ) {
  // get broadcast IP address
  IPAddress broadcastIp;
  broadcastIp = ~uint32_t(Network.subnetMask()) | uint32_t(Network.gatewayIP());

  AsyncUDPMessage outUDPMsg;
  msg.send(outUDPMsg);

  udp.sendTo(outUDPMsg, broadcastIp, OSC_DEFAULT_SEND_PORT);
}

void AsyncOSC::send( OSCMessage &msg, IPAddress & ip, int port) {
  // see: https://gitlab.doc.gold.ac.uk/physical-computing/osc_huzzah32/-/blob/master/osc_huzzah32.ino
  AsyncUDPMessage outUDPMsg;
  msg.send(outUDPMsg);

  udp.sendTo(outUDPMsg, ip, port);
}