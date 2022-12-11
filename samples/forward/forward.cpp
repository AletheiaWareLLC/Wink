#include <iostream>
#include <string>

#include <Wink/address.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 4) {
    error()
        << "Incorrect parameters, expected <spawner> <address> <destination>\n"
        << std::flush;
    return -1;
  }

  Address spawner(argv[1]);
  Address address(argv[2]);
  std::string name("forward/Forward");
  UDPSocket socket;
  Machine m(spawner, address, name, socket);

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
