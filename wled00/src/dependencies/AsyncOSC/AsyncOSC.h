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
#define OSC_DEFAULT_LISTEN_PORT   12345
#define OSC_DEFAULT_SEND_PORT     54321


//typedef void (*callbackOSC_c)(OSCMessage &); // c-style callback
typedef std::function<void(OSCMessage &)> callbackOSC;
//using callbackOSC = std::function<void(OSCMessage &)>; // c++ style callback


class AsyncOSC {
 private:
   AsyncUDP udp;

  std::map<std::string, std::function<void(OSCMessage &)>> oscmap; // maps addresses to handlers

   bool initUnicast(uint16_t port);
   // packet parser callback
   void parsePacket(AsyncUDPPacket _packet);

 public:
   AsyncOSC(); //osc_bundle_callback_function cbb, osc_message_callback_function cbm);

   // Generic UDP listener, no physical or IP configuration
   bool begin(uint16_t port = OSC_DEFAULT_LISTEN_PORT);

   void broadcast(OSCMessage &msg);
   void send(OSCMessage &msg, IPAddress &ip, int port = OSC_DEFAULT_SEND_PORT);

   void handleOscMessage(OSCMessage &msg);
   void handleOscBundle(OSCBundle &bndl);

   std::function<void(OSCMessage &)> getHandlerForAddress(std::string addr);
//   void addHandlerForAddress(std::string addr, callbackOSC_c cb);
//   void addHandlerForAddress(std::string addr, callbackOSC cb);
   void addHandlerForAddress(std::string addr, std::function<void(OSCMessage &)> cb);
};


#endif // __ASYNCOSC_H__