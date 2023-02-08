#pragma once

#include "wled.h"
#include <FastAccelStepper.h>

#define STEP_PIN 2
#define DIR_PIN 4
#define FAULT_PIN 35
#define STEPPER_ENABLE 14

int microStep = 32;
const int stepsPerRevolution = 200*microStep;

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

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
class DatPhotonUsermod : public Usermod {
  private:
    unsigned long lastTime = 0;

    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    bool sendHeartbeat = true;
    unsigned long msHeartbeat = 5000;
    int currentAccel = 50;
    int currentSpeed = 100;
    
    // unsigned long testULong = 42424242;
    // float testFloat = 42.42;
    // String testString = "Forty-Two";

    // // These config variables have defaults set inside readFromConfig()
    // int testInt;
    // long testLong;
    // int8_t testPins[2];

  public:

/*
bundle.dispatch("/moveto", on_moveto);
bundle.dispatch("/spin", on_spin);
bundle.dispatch("/stop", on_stop);
bundle.dispatch("/set_accel", on_set_accel);
bundle.dispatch("/set_speed", on_set_speed);
bundle.dispatch("/set_dir", on_set_dir);
*/

    void setupOscHandlers() {
      // configure callbacks for OSC messages handled in this usermod
      std::function<void(OSCMessage &)> fn1 = std::bind(&DatPhotonUsermod::on_move, this, std::placeholders::_1);
      osc.addHandlerForAddress("/motor/move", fn1 );

      std::function<void(OSCMessage &)> fn2 = std::bind(&DatPhotonUsermod::on_set_speed, this, std::placeholders::_1);
      osc.addHandlerForAddress("/motor/set_speed", fn2 );

      std::function<void(OSCMessage &)> fn3 = std::bind(&DatPhotonUsermod::on_stop, this, std::placeholders::_1);
      osc.addHandlerForAddress("/motor/stop", fn3 );      

      std::function<void(OSCMessage &)> fn4 = std::bind(&DatPhotonUsermod::on_set_accel, this, std::placeholders::_1);
      osc.addHandlerForAddress("/motor/set_accel", fn4 );

      std::function<void(OSCMessage &)> fn5 = std::bind(&DatPhotonUsermod::on_spin, this, std::placeholders::_1);
      osc.addHandlerForAddress("/motor/spin", fn5 );

      std::function<void(OSCMessage &)> fn6 = std::bind(&DatPhotonUsermod::on_set_dir, this, std::placeholders::_1);
      osc.addHandlerForAddress("/motor/set_dir", fn6 );
    }

    void setupStepper() {
      //setting microstepping to 32
      digitalWrite(STEPPER_ENABLE, HIGH);
      pinMode(STEP_PIN, OUTPUT);
      pinMode(DIR_PIN, OUTPUT);
      digitalWrite(STEP_PIN, LOW);
      digitalWrite(DIR_PIN, LOW);
      pinMode(FAULT_PIN, INPUT_PULLUP);
      pinMode(STEPPER_ENABLE, OUTPUT);
      digitalWrite(STEPPER_ENABLE, LOW);
      digitalWrite(STEPPER_ENABLE, LOW);

      // // jelle's test code for a full revolution 360
      // for(int i = 0; i<6400; i++){
      //   digitalWrite(STEP_PIN, HIGH);
      //   delay(1);
      //   digitalWrite(STEP_PIN, LOW);
      //   delay(1);
      // }

      engine.init();
      stepper = engine.stepperConnectToPin(STEP_PIN);
      if (stepper) {
        stepper->setDirectionPin(DIR_PIN);
        stepper->setEnablePin(STEPPER_ENABLE);
        stepper->setAutoEnable(true);

        // If auto enable/disable need delays, just add (one or both):
        // stepper->setDelayToEnable(50);
        // stepper->setDelayToDisable(1000);

        stepper->setSpeedInUs(20);  // the parameter is us/step !!!
        stepper->setAcceleration(5000);
      }

      stepper->move(3200);
      delay(50);
      stepper->move(-3200);
      delay(50);
      stepper->move(3200);
    } // setupStepper

    void setup() {
      Serial.println("[DAT] (P()h)oton) plugin loaded...");

      setupOscHandlers();
      setupStepper();
    }

    void on_stop(OSCMessage &msg) {

      // @TODO here is where I have to put the code, to move the stepper to the given position
      stepper->setSpeedInUs( 0 );
      Serial.println(" [DAT] /motor/stop ");
    }

    void on_set_accel(OSCMessage &msg) {
      int setAcc;

      if( msg.isInt(0) ) {
        setAcc = msg.getInt(0);
      } else {
        setAcc = floor(msg.getFloat(0));
      }

      Serial.println(" [DAT] /motor/set_accel ");
      Serial.print( setAcc );
      Serial.println();

      currentAccel = setAcc;
      // @TODO here is where I have to put the code, to move the stepper to the given position
      stepper->setAcceleration( currentAccel );
    }

    void on_move(OSCMessage &msg) {  
      // int range = stepsPerRevolution / 4;
      // int a = random(-range, range);

      int pos;

      if( msg.isInt(0) ) {
        pos = msg.getInt(0);
      } else {
        pos = floor(msg.getFloat(0));
      }

      Serial.print(" [DAT] /motor/move ");
      Serial.print( pos );
      Serial.println();

      stepper->move( pos );
    }

    void on_moveto(OSCMessage &msg) {
      int pos;

      if( msg.isInt(0) ) {
       pos = msg.getInt(0);
      } else {
        pos = floor(msg.getFloat(0));
      }

      Serial.println(" [DAT] /motor/moveto ");
      Serial.print( pos );
      
      stepper->moveTo(pos);
     //stepper.moveTo( random(0, 360) );
      stepper->setSpeedInUs( currentSpeed );
      stepper->setAcceleration( currentAccel );
    }

    void on_spin(OSCMessage &msg) {
      int speed, direction;
    
      if( msg.isInt(0) && msg.isInt(1) ) {
        speed = msg.getInt(0);
        direction = msg.getInt(1);
      } else {
        speed = floor(msg.getFloat(0));
        direction = floor(msg.getFloat(1));
      }

      Serial.print( speed );
      Serial.print( ", " );
      Serial.print( direction );
      Serial.println(" [DAT] /motor/spin ");

     stepper->setSpeedInUs( speed );
      //direction depends on wheter the value is under/above 0 
    }

    void on_set_speed(OSCMessage &msg) {
      // int range = stepsPerRevolution;
      // int a = random(-range, range);

      int vel;

      if( msg.isInt(0) ) {
        vel = msg.getInt(0);
      } else {
        vel = floor(msg.getFloat(0));
      }

      Serial.print(" [DAT] /motor/set_speed ");
      Serial.print( vel );
      Serial.println();

      currentSpeed = vel;

      stepper->setSpeedInUs( currentSpeed );
      // stepper->move(a);
    }

    void on_set_dir(OSCMessage &msg) {
      int dir;
      Serial.println(" [DAT] /motor/set_dir ");

      if( msg.isInt(0) ) {
      dir = msg.getInt(0);
      } else {
      dir = floor(msg.getFloat(0));
      }

      Serial.print( dir );
      Serial.println(" [DAT] /motor/set_dir ");

      currentSpeed *= -1;
    }



    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      //Serial.println("Connected to WiFi!");
    }

    void heartbeat() {
      //Serial.println(" [DAT] sending heartbeat...");
      OSCMessage msg("/fixture/heartbeat");
      msg.add( escapedMac.c_str() );        // unique identifier for fixture
      msg.add( strip.getLengthTotal() );    // length of strip in pixels?
      msg.add( e131Universe );              // DMX universe
      msg.add( DMXAddress );                // starting DMX channel
      // dispatch message to broadcast address
      osc.broadcast( msg );
    }

    unsigned long elapsed(unsigned long ms) { return ((millis() - lastTime) > ms); }
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

      if ( elapsed(msHeartbeat) && sendHeartbeat ) {
        heartbeat();
        lastTime = millis();
      }

    }


    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    /*
    void addToJsonInfo(JsonObject& root)
    {
      int reading = 20;
      //this code adds "u":{"Light":[20," lux"]} to the info object
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      JsonArray lightArr = user.createNestedArray("Light"); //name
      lightArr.add(reading); //value
      lightArr.add(" lux"); //unit
    }
    */


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root)
    {
      //root["user0"] = userVar0;
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {
      //userVar0 = root["user0"] | userVar0; //if "user0" key exists in JSON, update, else keep old value
      //if (root["bri"] == 255) Serial.println(F("Don't burn down your garage!"));
    }


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browser.
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     * 
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("LUMOKINETIX");
      //top["great"] = userVar0; //save these vars persistently whenever settings are saved
      top["sendHeartbeat"] = sendHeartbeat;
      top["msHeartbeat"] = msHeartbeat;
      // top["testInt"] = testInt;
      // top["testLong"] = testLong;
      // top["testULong"] = testULong;
      // top["testFloat"] = testFloat;
      // top["testString"] = testString;
      // JsonArray pinArray = top.createNestedArray("pin");
      // pinArray.add(testPins[0]);
      // pinArray.add(testPins[1]); 
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     * 
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     * 
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     * 
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

      JsonObject top = root["LUMOKINETIX"];

      bool configComplete = !top.isNull();

      //configComplete &= getJsonValue(top["great"], userVar0);
      configComplete &= getJsonValue(top["sendHeartbeat"], sendHeartbeat);
      configComplete &= getJsonValue(top["msHeartbeat"], msHeartbeat);
      // configComplete &= getJsonValue(top["testFloat"], testFloat);
      // configComplete &= getJsonValue(top["testString"], testString);

      // // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
      // configComplete &= getJsonValue(top["testInt"], testInt, 42);  
      // configComplete &= getJsonValue(top["testLong"], testLong, -42424242);
      // configComplete &= getJsonValue(top["pin"][0], testPins[0], -1);
      // configComplete &= getJsonValue(top["pin"][1], testPins[1], -1);

      return configComplete;
    }


    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw()
    {
      //strip.setPixelColor(0, RGBW32(0,0,0,0)) // set the first pixel to black
    }

   
    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_DAT_PHOTON;
    }

   //More methods can be added in the future, this example will then be extended.
   //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};