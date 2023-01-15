#include <iostream>
#include <string>

#include <wiringPi.h>

#include <Wink/address.h>
#include <Wink/log.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 4) {
    error() << "Incorrect parameters, expected <spawner> <address> <pin>\n"
            << std::flush;
    return -1;
  }

  std::string name(argv[0]);
  UDPSocket socket;
  Address address(argv[1]);
  Address spawner(argv[2]);
  Machine m(name, socket, address, spawner);

  wiringPiSetupGpio();

  int pin = std::stoi(argv[3]);
  pinMode(pin, INPUT);

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      []() {},
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"high",
           [&](const Address &sender, std::istream &args) {
             digitalWrite(pin, HIGH);
             info() << "GPIO " << pin << " is HIGH\n" << std::flush;
           }},
          {"low",
           [&](const Address &sender, std::istream &args) {
             digitalWrite(pin, LOW);
             info() << "GPIO " << pin << " is LOW\n" << std::flush;
           }},
      }));

  m.Start();
}
