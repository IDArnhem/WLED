#ifndef __ASYNCOSC_H__
#define __ASYNCOSC_H___

#ifdef ESP32
#include <WiFi.h>
#include <AsyncUDP.h>
#elif defined (ESP8266)
#include <ESPAsyncUDP.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#else
#error Platform not supported
#endif

#include <Arduino.h>

#include "OSC/OSCMessage.h"
#include "OSC/OSCBundle.h"

#include <map>


// defaults
#define OSC_DEFAULT_PORT   12345

// new packet callback
// typedef void (*osc_bundle_callback_function) (OSCBundle &bndl);
// typedef void (*osc_message_callback_function) (OSCMessage &msg);


typedef void (*callbackOSC_c)(OSCMessage &); // c-style callback
typedef std::function<void(OSCMessage &)> callbackOSC;
//using callbackOSC = std::function<void(OSCMessage &)>; // c++ style callback


class AsyncOSC {
 private:
   AsyncUDP udp;

  std::map<std::string, std::function<void(OSCMessage &)>> oscmap; // maps addresses to handlers

  //std::map<std::string, callbackOSC_c> oscmap; // maps addresses to handlers

   bool initUnicast(uint16_t port);
   // packet parser callback
   void parsePacket(AsyncUDPPacket _packet);

   //osc_bundle_callback_function _cbBundle = nullptr;
   //osc_message_callback_function _cbMessage = nullptr;

 public:
   AsyncOSC(); //osc_bundle_callback_function cbb, osc_message_callback_function cbm);

   // Generic UDP listener, no physical or IP configuration
   bool begin(uint16_t port = OSC_DEFAULT_PORT);

   void handleOscMessage(OSCMessage &msg);
   void handleOscBundle(OSCBundle &bndl);

   std::function<void(OSCMessage &)> getHandlerForAddress(std::string addr);
   void addHandlerForAddress(std::string addr, callbackOSC_c cb);
//   void addHandlerForAddress(std::string addr, callbackOSC cb);
   void addHandlerForAddress(std::string addr, std::function<void(OSCMessage &)> cb);
};


#endif // __ASYNCOSC_H__