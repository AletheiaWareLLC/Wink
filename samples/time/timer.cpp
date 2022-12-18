#include <iostream>
#include <string>

#include <Wink/address.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    error() << "Incorrect parameters, expected <spawner> <address> <duration>\n"
            << std::flush;
    return -1;
  }

  Address spawner(argv[1]);
  Address address(argv[2]);
  UDPSocket socket;
  Machine m(spawner, address, "time/Timer", socket);

  std::istringstream iss(argv[3]);
  int seconds;
  iss >> seconds;
  std::time_t start = std::time(nullptr);

  m.AddState(std::make_unique<State>(
      // State Name
      "idle",
      // Parent State
      "",
      // On Entry Action
      []() { info() << "Timer is IDLE\n"
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
          {"stop",
           [&](const Address &sender, std::istream &args) { m.Exit(); }},
          {"exit",
           [&](const Address &sender, std::istream &args) { m.Exit(); }},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "timing",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        const auto now = std::time(nullptr);
        const auto elapsed = now - start;
        info() << "Timer is TIMING: "<<elapsed<<'\n' << std::flush;
        if (elapsed < seconds) {
          sleep(1);
          m.GotoState("timing"); // Loop
        } else {
          m.SendSpawner("timeup");
          m.Exit();
        }
      },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
      }));

  m.Start("timing");
}
