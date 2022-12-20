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

  int seconds;
  std::time_t start;

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
             args >> seconds;
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
      "idle",
      // On Entry Action
      [&]() { m.SendSelf("update"); },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"update", [&](const Address &sender, std::istream &args) {
             const auto now = std::time(nullptr);
             const auto elapsed = now - start;
             const auto remaining = seconds - elapsed;
             info() << "Timer is TIMING: " << remaining << "s left\n"
                    << std::flush;
             if (remaining > 0) {
               m.SendSelf("update"); // Loop
             } else {
               m.SendSpawner("timeup");
               m.Exit();
             }
           }}}));

  if (argc == 4) {
    // Start automatically
    std::istringstream iss(argv[3]);
    iss >> seconds;
    start = std::time(nullptr);
    m.Start("timing");
  } else {
    m.Start(); // Start in Idle
  }
}
