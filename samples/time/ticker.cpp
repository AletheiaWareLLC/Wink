#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <Wink/address.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 4) {
    error() << "Incorrect parameters, expected <address> <spawner> <interval>\n"
            << std::flush;
    return -1;
  }

  std::string name(argv[0]);
  UDPSocket socket;
  Address address(argv[1]);
  Address spawner(argv[2]);
  Machine m(name, socket, address, spawner);

  const std::chrono::seconds interval(std::stoi(argv[3]));

  std::atomic_bool running = true;

  std::thread worker([&]() {
    while (running) {
      std::this_thread::sleep_for(interval);
      if (running) {
        m.Send(spawner, "tick");
      }
    }
  });

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
          {"exit",
           [&](const Address &sender, std::istream &args) {
             running = false;
             worker.join();
             m.Exit();
           }},
      }));

  m.Start();
}
