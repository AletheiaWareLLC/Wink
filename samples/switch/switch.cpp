#include <iostream>
#include <string>

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
  Machine m(spawner, address, "switch/Switch", socket);

  m.AddState(std::make_unique<State>(
      // State Name
      "off",
      // Parent State
      "",
      // On Entry Action
      []() { info() << "Switch is OFF\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"on", [&](const Address &sender,
                     std::istream &args) { m.GotoState("on"); }},
          {"off", [&](const Address &sender,
                      std::istream &args) { m.GotoState("off"); }},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "on",
      // Parent State
      "off",
      // On Entry Action
      []() { info() << "Switch is ON\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{}));

  m.Start();
}
