#include <iostream>
#include <string>

#include <Wink/address.h>
#include <Wink/delay.h>
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
  Machine m(spawner, address, "delay/Delay", socket);

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        // Wait 5s then exit
        After(5, [&]() { m.SendSelf("exit"); });
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
