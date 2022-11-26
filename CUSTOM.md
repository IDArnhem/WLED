# Custom WLED-DAT

This is made for the light installation with the third year's of Design, Art and Technology

Divergences with conventional WLED:
- added OSC support using Async UDP
- configurable OSC ports are not yet in the web UI, so at the moment it listens on 12345 and sends on 54321
- created a usermod to control other features in a fixture
- changed the platformio.ini file to include some debugging information
- included FastStepperAccel library to control a stepper motor
- added a usermod to control de Photon fixtures that feature a LED strip and a stepper motor

