# Our DAT Usermod

Here we control the kinetic fixtures.

## How to

Get copy of the source:

`git clone https://github.com/IDArnhem/WLED.git`

change to `custom` branch:

`git checkout custom`

follow the build instructions on [this page](https://kno.wled.ge/advanced/compiling-wled/)

device listens for OSC messages on port `12345`, and sends a hearbeat message to the broadcast 
address to all devices in the network that listen on port `54321`. 

This is the OSC message map for the motor driver:

| OSC | param | description |
|---|---|---|
| /motor/move | int *steps* | move motor <steps> |
| /motor/set_speed | int *speed* | set speed of motion |
| /motor/stop | n.a. | sets speed to zero |
| /motor/set_accel | int *steps per second* | set acceleration of easing function |
| /motor/spin | n.a. | spin motor continuously |
| /motor/set_dir | | not implemented |
