#pragma once

#include "wled.h"

// ////////////////////////////////////////////////////////////////////////////
//  ▄▄▄▄▄▄  ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ 
// █      ██       █       █
// █  ▄    █   ▄   █▄     ▄█
// █ █ █   █  █▄█  █ █   █  
// █ █▄█   █       █ █   █  
// █       █   ▄   █ █   █  
// █▄▄▄▄▄▄██▄▄█ █▄▄█ █▄▄▄█  
//
// |l u m o k i n e t i x|
// ////////////////////////////////////////////////////////////////////////////
class DatHeartbeatUsermod : public Usermod {
  private:
    unsigned long lastTime = 0;

    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    bool sendHeartbeat = true;
    unsigned long msHeartbeat = 5000;
  
  public:

    void setup() {
      Serial.println("[DAT] Heartbeat <3");
    }

    void heartbeat() {
      //Serial.println(" [DAT] sending heartbeat...");
      OSCMessage msg("/fixture/heartbeat");
      msg.add( escapedMac.c_str() );        // unique identifier for fixture
      msg.add( serverDescription );         // user-given fixture name
      msg.add( strip.getLengthTotal() );    // length of strip in pixels?
      msg.add( e131Universe );              // DMX universe
      msg.add( DMXAddress );                // starting DMX channel
      // dispatch message to broadcast address
      osc.broadcast( msg );
    }

    unsigned long elapsed(unsigned long ms) { return ((millis() - lastTime) > ms); }

    void loop() {

      if ( elapsed(msHeartbeat) && sendHeartbeat ) {
        heartbeat();
        lastTime = millis();
      }

    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("OSCHEARTBEAT");
      top["sendHeartbeat"] = sendHeartbeat;
      top["msHeartbeat"] = msHeartbeat;
    }


    bool readFromConfig(JsonObject& root)
    {
      JsonObject top = root["OSCHEARTBEAT"];

      bool configComplete = !top.isNull();

      //configComplete &= getJsonValue(top["great"], userVar0);
      configComplete &= getJsonValue(top["sendHeartbeat"], sendHeartbeat);
      configComplete &= getJsonValue(top["msHeartbeat"], msHeartbeat);

      return configComplete;
    }

    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_DAT_HEARTBEAT;
    }

}; // class