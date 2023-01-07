# Raspberry Pi

This is a collection of samples showing how to use the Wink framework to build and run State Machines on the Raspberry Pi platform.

These samples assume your Raspberry Pi is powered up, connected to your local network, running the latest version of Raspbian OS, and has the latest version of the [WiringPi](http://wiringpi.com/) library installed.

First, clone the Wink repository, build the project, and start the server;
```
# Clone Wink Repository
git clone https://github.com/AletheiaWareLLC/Wink.git

# Change Directory
cd Wink

# Build Wink Framework, including the Raspberry Pi samples which are not built by default
cmake -S . -B build
cmake --build build -DBUILD_RPI_SAMPLES=ON

# Start Wink Server
./build/src/WinkServer serve build/samples/
```

## Blinky

The sample has two LEDs and a button, and utilizes three State Machines;
- GPIOWriter - drives a given GPIO high/low to turn an LED on/off based on the message it receives.
- GPIOReader - reads a given GPIO to get the state of the button.
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
