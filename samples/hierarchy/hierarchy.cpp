#include <iostream>
#include <string>

#include <Wink/address.h>
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
  Machine m(spawner, address, "hierarchy/Hierarchy", socket);

  m.AddState(std::make_unique<State>(
      // State Name
      "Parent",
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
             info() << "Parent: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "Child",
      // Parent State
      "Parent",
      // On Entry Action
      []() { info() << "Child On Entry\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"Test",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Child: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.Start("Child");
}
