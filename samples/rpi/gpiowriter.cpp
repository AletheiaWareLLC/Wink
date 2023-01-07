#include <iostream>
#include <string>

#include <wiringPi.h>

#include <Wink/address.h>
#include <Wink/log.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    error() << "Incorrect parameters, expected <spawner> <address>\n"
            << std::flush;
    return -1;
  }

  Address spawner(argv[1]);
  Address address(argv[2]);
  UDPSocket socket;
  Machine m(spawner, address, "rpi/GPIOWriter", socket);

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      []() { wiringPiSetupGpio(); },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"high",
           [&](const Address &sender, std::istream &args) {
             int pin;
             args >> pin;
             pinMode(pin, OUTPUT);
             digitalWrite(pin, HIGH);
             info() << "GPIO " << pin << " is HIGH\n" << std::flush;
           }},
          {"low",
           [&](const Address &sender, std::istream &args) {
             int pin;
             args >> pin;
             pinMode(pin, OUTPUT);
             digitalWrite(pin, LOW);
             info() << "GPIO " << pin << " is LOW\n" << std::flush;
           }},
      }));

  m.Start();
}
