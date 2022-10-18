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


// defaults
#define OSC_DEFAULT_PORT   12345

// new packet callback
typedef void (*osc_callback_function) (OSCMessage &msg);


class AsyncOSC {
 private:
    AsyncUDP udp;

    bool initUnicast(uint16_t port);
    // packet parser callback
    void parsePacket(AsyncUDPPacket _packet);
    
    osc_callback_function _callback = nullptr;

 public:
    AsyncOSC(osc_callback_function callback);

    // Generic UDP listener, no physical or IP configuration
    bool begin(uint16_t port = OSC_DEFAULT_PORT);
};


#endif // __ASYNCOSC_H__