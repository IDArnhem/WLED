#pragma once

#include "wled.h"

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
class MurgiaUsermod : public Usermod {
  private:
    bool overridetube = false;

    unsigned long lastTime = 0;

    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    bool sendHeartbeat = true;
    unsigned long msHeartbeat = 5000;

    int powerl = 0;
    int modenum;
    long bright = 0;
    // long speedswipe = 0;
    // long colorswipe = 0;
    // long colorswipe2 = 0;
    // long colorswipe3 = 0;
    // long colorswipe4 = 0;
    // long colorswipe5 = 0;
    // long colorswipe6 = 0;

    long startedWhen;

    float colR = 0;
    float colG = 0;
    float colB = 0;
    float freq = 0;

    // bool osc = false;


  public:
    void setupOscHandlers() {
      // configure callbacks for OSC messages handled in this usermod
      std::function<void(OSCMessage &)> fn1 = std::bind(&MurgiaUsermod::on_tube, this, std::placeholders::_1);
      osc.addHandlerForAddress("/tube", fn1 );

      std::function<void(OSCMessage &)> fn2 = std::bind(&MurgiaUsermod::on_litemode, this, std::placeholders::_1);
      osc.addHandlerForAddress("/litemode", fn2 );

      std::function<void(OSCMessage &)> fn3 = std::bind(&MurgiaUsermod::on_litelevel, this, std::placeholders::_1);
      osc.addHandlerForAddress("/litelevel", fn3 );

      // configure callbacks for OSC messages handled in this usermod
      std::function<void(OSCMessage &)> fn4 = std::bind(&MurgiaUsermod::on_override, this, std::placeholders::_1);
      osc.addHandlerForAddress("/override", fn4 );
    }

    void setup() {
      Serial.println("MMMURGIA plugin loaded...");

      setupOscHandlers();
    }

    // clean port of Federico's strobe code
    void fastrobetimer(int frequ,int r,int g,int b )
    {
      int strobe_delay = 1000000/(frequ+1);
      int on_time = 10000;

      if((micros() - startedWhen) < strobe_delay) 
      {
        if((micros() - startedWhen) < on_time) 
        {
          fill(r,g,b);
        }
        else
          fill(0,0,0);
      }
      else {
        startedWhen = micros();
      }
    }

    void colorpitch(int csw, int r, int g, int b) {
      // find which is smallest csw      
      int upperlimit = MIN(csw, strip.getLengthTotal());
      
      for (int i = 0; i < csw; i++){
        //fastPixel(i, colorswipe2,colorswipe3,colorswipe4);
        strip.setPixelColor(i, r, g, b);
      }
      
      for (int n=csw; n < strip.getLengthTotal(); n++){
        // fastPixel(n, 0,0,0);    
        strip.setPixelColor(n, 0, 0, 0);
      }
    }

    void sinelon(int freq, int r, int g, int b)
    {
      int reset = 0;
      static uint32_t local_timebase = 0;
      uint32_t last_reset = 0;
      int phase = 0;
      
      reset = powerl;

      if (reset == 1) {
          last_reset = millis();
      }

      local_timebase = millis() - last_reset;
      
      int tailsize = 20;
      int cscale = 120/freq;

      // void 	fadeToBlackBy (CRGB *leds, uint16_t num_leds, uint8_t fadeBy)
      //fadeToBlackBy( leds, tailsize, cscale); // 2nd number led count
      for( uint16_t i = 0; i < tailsize; ++i) {
        CRGB v = CRGB(strip.getPixelColor(i));
        v.nscale8(cscale);
        strip.setPixelColor(i, v);
      }

      // SEE SINELON in WLED FX for the tail effect

      int pos = beatsin16( freq*2, 0, 20, local_timebase, phase);
      
      //leds[pos] += CRGB( colorswipe2,colorswipe3,colorswipe4);
      strip.setPixelColor(pos, r, g, b);
    }

/*
void fadeToBlackBy( CRGB* leds, uint16_t num_leds, uint8_t fadeBy)
{
    nscale8( leds, num_leds, 255 - fadeBy);
}

void nscale8( CRGB* leds, uint16_t num_leds, uint8_t scale)
{
    for( uint16_t i = 0; i < num_leds; ++i) {
        leds[i].nscale8( scale);
    }
}
*/
    // fill the tube with light of RGB value
    void fill(int r, int g, int b) {
      for(int i=0; i < strip.getLengthTotal(); i++) {
        strip.setPixelColor(i, r, g, b);
      }
    }

    // ///////////////////////////////////////////////////////////////////////
    void on_tube(OSCMessage &msg) {
      char tubeid[33];

      if(msg.isString(0)){
        msg.getString(0, tubeid, msg.getDataLength(0) );

        if( strcmp(serverDescription, tubeid) == 0 ) {
          if(msg.isFloat(1)){
            freq = msg.getFloat(1);
            if(freq < 0) { freq = .0; }
          }
          
          if(msg.isFloat(2)){
            colR = msg.getFloat(2);
          }
          
          if(msg.isFloat(3)){
            colG = msg.getFloat(3);
          }
          
          if(msg.isFloat(4)){
            colB = msg.getFloat(4);
          }

          Serial.print(">> RGB ");
          Serial.print(tubeid);
          Serial.print(", ");
          Serial.print(freq);
          Serial.print(", ");
          Serial.print(colR);
          Serial.print(", ");
          Serial.print(colG);
          Serial.print(", ");
          Serial.print(colB);
          Serial.println();
        } else {
          //Serial.println("This message is NOT for me!!!");
        }
      }
    }

    void on_litemode(OSCMessage &msg) {
      modenum = int(msg.getFloat(0));
      Serial.print(">> LITEMODE ");
      Serial.print(modenum);
      Serial.println();
    }

    void on_litelevel(OSCMessage &msg) {
      powerl = int(msg.getFloat(0));
      Serial.print(">> LITELEVEL ");
      Serial.print(powerl);
      Serial.println();
    }

    void on_override(OSCMessage &msg) {
      if( msg.isBoolean(0) ) {
        overridetube = msg.getBoolean(0);
      }

      if( msg.isInt(0) ) {
        int val = msg.getInt(0);
        overridetube = (val == 1);
      }
    }

    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      Serial.println("Murgia is connected to WiFi!");
      Serial.print("<< I AM FIXTURE [");
      Serial.print(serverDescription);
      Serial.println("]");
    }

    // the heartbeat has its own usermod now

    // void heartbeat() {
    //   //Serial.println(" [DAT] sending heartbeat...");
    //   OSCMessage msg("/fixture/heartbeat");
    //   msg.add( escapedMac.c_str() );        // unique identifier for fixture
    //   msg.add( serverDescription );         // user-given fixture name
    //   msg.add( strip.getLengthTotal() );    // length of strip in pixels?
    //   msg.add( e131Universe );              // DMX universe
    //   msg.add( DMXAddress );                // starting DMX channel
    //   // dispatch message to broadcast address
    //   osc.broadcast( msg );
    // }

    // unsigned long elapsed(unsigned long ms) { return ((millis() - lastTime) > ms); }
    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     * 
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() {

      // if ( elapsed(msHeartbeat) && sendHeartbeat ) {
      //   heartbeat();
      //   lastTime = millis();
      // }


      if( overridetube ) {
        if(modenum == 0) //  MODE SET 0
        { 
          fastrobetimer(freq, colR, colG, colB);
        } else if (modenum == 1) //  MODE SET 1 
        {
          colorpitch(int(freq), colR, colG, colB);
        }
        else if (modenum == 2) //  MODE SET 1 
        {
          sinelon(int(freq), colR, colG, colB);
        }

        strip.show();
      }
    } // loop
   
    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_MURGIA;
    }

}; // class