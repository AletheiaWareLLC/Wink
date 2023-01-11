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

  std::chrono::time_point<std::chrono::system_clock> start;

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
             start = std::chrono::system_clock::now();
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
             const auto now = std::chrono::system_clock::now();
             const auto delta =
                 std::chrono::floor<std::chrono::seconds>(now - start).count();
             std::ostringstream oss;
             oss << "elapsed ";
             oss << delta;
             oss << " seconds";
             m.Send(sender, oss.str());
             m.GotoState("idle");
           }},
      }));

  m.Start();
}
