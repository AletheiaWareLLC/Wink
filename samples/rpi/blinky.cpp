#include <chrono>
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

  std::string name(argv[0]);
  UDPSocket socket;
  Address address(argv[1]);
  Address spawner(argv[2]);
  Machine m(name, socket, address, spawner);

  const auto reader = "rpi/GPIOReader";
  const auto writer = "rpi/GPIOWriter";

  const auto buttonPin = 10;
  const auto led1Pin = 11;
  const auto led2Pin = 12;

  auto led1on = false;

  std::map<const std::string, const Address &> addresses;

  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        m.Spawn(reader);
        m.Spawn(writer);
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
             addresses.insert(child, sender);
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
             addresses.remove(child);
             m.Spawn(child); // Retry
           }},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "loop",
      // Parent State
      "main",
      // On Entry Action
      []() {
        // Request Button state
        if (const auto a = addresses.find(reader); a != addresses.end()) {
          std::ostringstream oss;
          oss << "read";
          oss << buttonPin;
          m.Send(a, oss.str());
        }
        // Periodically toggle LED1
        led1on = !led1on;
        if (const auto a = addresses.find(writer); a != addresses.end()) {
          std::ostringstream oss;
          oss << led1on ? "high " : "low ";
          oss << led1Pin;
          m.Send(a, oss.str());
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
             if (const auto a = addresses.find(writer);
                 a != addresses.end() && p == buttonPin) {
               // Switch LED2 based on Button state
               int state;
               args >> state;
               std::ostringstream oss;
               oss << state ? "high " : "low ";
               oss << led2Pin;
               m.Send(a, oss.str());
             }
           }},
          {"loop", [&](const Address &sender,
                       std::istream &args) { m.GotoState("loop"); }},
      }));

  m.Start();
}
