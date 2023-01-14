# Raspberry Pi

This is a collection of samples showing how to use the Wink framework to build and run State Machines on the Raspberry Pi platform.

First, power up your Raspberry Pi, connect it to your local network, update to the latest version of Raspbian OS and install the latest version of [Clang](https://clang.llvm.org/), [CMake](https://cmake.org/), and [WiringPi](http://wiringpi.com/).

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install clang cmake wiringpi
```

Then, clone the Wink repository, build the project, and finally start the server;
```
# Clone Wink Repository
git clone https://github.com/AletheiaWareLLC/Wink.git

# Change Directory
cd Wink

# Build Wink Framework, including the Raspberry Pi samples which are not built by default
cmake -S . -B build -D BUILD_RPI_SAMPLES=ON
cmake --build build

# Start Wink Server
./build/src/WinkServer serve build/samples/
```

Now that you have ensured everything is working, unplug your Raspberry Pi so you can safely connect up the hardware as described below. Once connected, power up your Raspberry Pi and restart the Wink server.

## Blinky

The sample uses two LEDs and a button, driven by a system comprising three State Machines;
- GPIOWriter - responds to requests to drive a given GPIO high/low to turn an LED on/off.
- GPIOReader - responds to requests to read a given GPIO to get the state of the button.
- Blinky - encapsulates the business logic of switching LED1 periodically and controlling LED2 with the Button.

### Hardware

This sample will use the same hardware setup as the GPIO tutorial by [SparkFun](https://learn.sparkfun.com/tutorials/raspberry-gpio).

![Wiring Diagram](https://cdn.sparkfun.com/assets/learn_tutorials/4/2/4/raspberry-gPI2o-circuit2_bb2.jpg)

### Software

From another computer on the same network, run;
```
./build/src/Wink start rpi/Blinky <Raspberry-Pi-IP-Address>
```

If all goes well, one LED will flash periodically and the other will turn on whenever the button is held down.
