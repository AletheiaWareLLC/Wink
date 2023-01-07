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
  Machine m(spawner, address, "rpi/GPIOReader", socket);

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
          {"read",
           [&](const Address &sender, std::istream &args) {
             int pin;
             args >> pin;
             pinMode(pin, INPUT);

             std::ostringstream oss;
             oss << "gpio ";
             oss << pin;
             if (digitalRead(pin)) {
               oss << " 1";
               info() << "GPIO " << pin << " is HIGH\n" << std::flush;
             } else {
               oss << " 0";
               info() << "GPIO " << pin << " is LOW\n" << std::flush;
             }
             m.Send(sender, oss.str());
           }},
      }));

  m.Start();
}
