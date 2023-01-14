#include <chrono>
#include <iostream>
#include <string>

#include <Wink/address.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    error() << "Incorrect parameters, expected <address> <spawner>\n"
            << std::flush;
    return -1;
  }

  std::string name(argv[0]);
  UDPSocket socket;
  Address address(argv[1]);
  Address spawner(argv[2]);
  Machine m(name, socket, address, spawner);

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        // Schedule message to be sent to self after 10s
        m.SendAfter(address, "exit", std::chrono::seconds(10));
      },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"exit",
           [&](const Address &sender, std::istream &args) { m.Exit(); }},
      }));

  m.Start();
}
