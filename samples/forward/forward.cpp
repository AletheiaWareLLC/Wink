#include <iostream>
#include <string>

#include <Wink/address.h>
#include <Wink/log.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 4) {
    error()
        << "Incorrect parameters, expected <address> <spawner> <destination>\n"
        << std::flush;
    return -1;
  }

  std::string name(argv[0]);
  UDPSocket socket;
  Address address(argv[1]);
  Address spawner(argv[2]);
  Machine m(name, socket, address, spawner);

  Address destination(argv[3]);

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
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             m.Send(destination, os.str());
           }},
      }));

  m.Start();
}
