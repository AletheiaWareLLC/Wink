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
  Machine m(spawner, address, "time/Delay", socket);

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        info() << "Delay OnEntry\n" << std::flush;
        m.Spawn("time/Timer", std::vector<std::string>{"5s"}); // Start 5s Timer
      },
      // On Exit Action
      []() { info() << "Delay OnExit\n"
                    << std::flush; },
      // Receivers
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Delay: " << sender << " : " << os.str() << '\n'
                    << std::flush;
           }},
          {"timeout",
           [&](const Address &sender, std::istream &args) {
             info() << "Delay: " << sender << " has timed out\n" << std::flush;
             m.Exit();
           }},
      }));

  m.Start();
}
