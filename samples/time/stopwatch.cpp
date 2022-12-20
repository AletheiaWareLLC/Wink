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
  Machine m(spawner, address, "time/StopWatch", socket);

  std::time_t start;

  m.AddState(std::make_unique<State>(
      // State Name
      "idle",
      // Parent State
      "",
      // On Entry Action
      []() { info() << "StopWatch is IDLE\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"idle", [&](const Address &sender,
                       std::istream &args) { m.GotoState("idle"); }},
          {"start",
           [&](const Address &sender, std::istream &args) {
             start = std::time(nullptr);
             m.GotoState("timing");
           }},
          {"stop", [&](const Address &sender,
                       std::istream &args) { m.GotoState("idle"); }},
          {"exit",
           [&](const Address &sender, std::istream &args) { m.Exit(); }},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "timing",
      // Parent State
      "idle",
      // On Entry Action
      []() { info() << "StopWatch is TIMING\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"stop",
           [&](const Address &sender, std::istream &args) {
             std::time_t now = std::time(nullptr);
             std::ostringstream oss;
             oss << "elapsed ";
             oss << (now - start);
             m.Send(sender, oss.str());
             m.GotoState("idle");
           }},
      }));

  m.Start();
}
