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
    // unsigned long lastTime = 0;

    // // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    // bool sendHeartbeat = true;
    // unsigned long msHeartbeat = 5000;
  
    int currentAccel = 50;
    int currentSpeed = 100; // Hz = steps/s
    int currentDirection = 1; // 1 = forward, -1 = backward
    
  public:

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

        // stepper->setSpeedInUs(20);  // the parameter is us/step !!!
        // stepper->setAcceleration(5000);

        stepper->setSpeedInHz( 6400 ); // steps/s
        stepper->setAcceleration( 500 );
      }

      int stepsHalfTurn = stepsPerRevolution / 2;

      stepper->move(stepsHalfTurn);
      delay(50);
      stepper->move(-stepsHalfTurn);
      delay(50);
      stepper->move(stepsHalfTurn);
    } // setupStepper

    void setup() {
      Serial.println("[DAT] (P()h)oton) plugin loaded...");

      setupOscHandlers();
      setupStepper();
    }

    void on_stop(OSCMessage &msg) {
      Serial.println(" [DAT] /motor/stop ");
      stepper->stopMove();
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

      stepper->setSpeedInHz( currentSpeed );
      stepper->setAcceleration( currentAccel );
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
      
      stepper->setSpeedInHz( currentSpeed );
      stepper->setAcceleration( currentAccel );
      stepper->moveTo(pos);
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

      stepper->setSpeedInHz( currentSpeed );
      stepper->setAcceleration( currentAccel );

      currentDirection = direction;

      //direction depends on wheter the value is under/above 0 
      if(direction > 0) {
        stepper->runForward();
      } else {
        stepper->runBackward();
      }
    }

    void on_set_speed(OSCMessage &msg) {
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

      stepper->setSpeedInHz( currentSpeed );
    }

    void on_set_dir(OSCMessage &msg) {
      int dir;

      if( msg.isInt(0) ) {
        dir = msg.getInt(0);
      } else {
        dir = floor(msg.getFloat(0));
      }

      currentDirection = dir;

      Serial.print(" [DAT] /motor/set_dir ");
      Serial.println( dir );
    }



    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      //Serial.println("Connected to WiFi!");
    }

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
    }


 
    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_DAT_PHOTON;
    }

};