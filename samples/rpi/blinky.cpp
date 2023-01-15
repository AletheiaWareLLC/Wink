#include <chrono>
#include <iostream>
#include <string>

#include <wiringPi.h>

#include <Wink/address.h>
#include <Wink/machine.h>
#include <Wink/state.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    error() << "Incorrect parameters, expected <spawner> <address>\n"
            << std::flush;
    return -1;
  }

  std::string name(argv[0]);
  UDPSocket socket;
  Address address(argv[1]);
  Address spawner(argv[2]);
  Machine m(name, socket, address, spawner);

  const auto buttonReader = "rpi/GPIOReader:button";
  const auto led1Writer = "rpi/GPIOWriter:led1";
  const auto led2Writer = "rpi/GPIOWriter:led2";

  const auto buttonPin = 17;
  const auto led1Pin = 18;
  const auto led2Pin = 23;

  auto led1on = false;

  std::map<const std::string, const Address > addresses;

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        m.Spawn(buttonReader, std::vector<std::string>{std::to_string(buttonPin), std::to_string(PUD_UP)});
        m.Spawn(led1Writer, std::vector<std::string>{std::to_string(led1Pin)});
        m.Spawn(led2Writer, std::vector<std::string>{std::to_string(led2Pin)});
        m.GotoState("loop");
      },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"started",
           [&](const Address &sender, std::istream &args) {
             std::string child;
             args >> child;
             info() << "Parent: " << sender << ' ' << child << " has started\n"
                    << std::flush;
             addresses.emplace(child, sender);
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
             addresses.erase(child);
             m.Spawn(child); // Retry
           }},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "loop",
      // Parent State
      "main",
      // On Entry Action
      [&]() {
        // Request Button state
        if (const auto &a = addresses.find(buttonReader); a != addresses.end()) {
          m.Send(a->second, "read");
        }
        // Periodically toggle LED1
        led1on = !led1on;
        if (const auto &a = addresses.find(led1Writer); a != addresses.end()) {
          m.Send(a->second, (led1on ? "high" : "low"));
        }
        // Schedule message to be sent to self after 1s
        m.SendAfter(address, "loop", std::chrono::seconds(1));
      },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"gpio",
           [&](const Address &sender, std::istream &args) {
             int p;
             args >> p;
             if (const auto &a = addresses.find(led2Writer);
                 a != addresses.end() && p == buttonPin) {
               // Switch LED2 based on Button state
               int state;
               args >> state;
               m.Send(a->second, (state ? "high" : "low"));
             }
           }},
          {"loop", [&](const Address &sender,
                       std::istream &args) { m.GotoState("loop"); }},
      }));

  m.Start();
}
