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
  Machine m(spawner, address, "family/Parent", socket);

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        info() << "Parent: OnEntry\n" << std::flush;
        m.Spawn("family/Child");
      },
      // On Exit Action
      []() { info() << "Parent: OnExit\n"
                    << std::flush; },
      // Receivers
      std::map<const std::string, Receiver>{
          {"started",
           [&](const Address &sender, std::istream &args) {
             std::string child;
             args >> child;
             info() << "Parent: " << sender << ' ' << child << " has started\n"
                    << std::flush;
           }},
          {"pulsed",
           [&](const Address &sender, std::istream &args) {
             std::string child;
             args >> child;
             info() << "Parent: " << sender << ' ' << child << " has pulsed\n"
                    << std::flush;
           }},
          {"errored",
           [&](const Address &sender, std::istream &args) {
             std::string child;
             args >> child;
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Parent: " << sender << ' ' << child
                    << " has errored: " << os.str() << '\n'
                    << std::flush;
           }},
          {"exited",
           [&](const Address &sender, std::istream &args) {
             std::string child;
             args >> child;
             info() << "Parent: " << sender << ' ' << child << " has exited\n"
                    << std::flush;
             m.GotoState("main"); // Retry
           }},
      }));

  m.Start();
}
