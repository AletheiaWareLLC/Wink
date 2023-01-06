#include <iostream>
#include <string>

#include <Wink/address.h>
#include <Wink/log.h>
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
  Machine m(spawner, address, "fizzbuzz/FizzBuzz", socket);

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
          {"",
           [&](const Address &sender, std::istream &args) {
             int n;
             args >> n;
             if (n % 15 == 0) {
               m.GotoState("FizzBuzz");
             } else if (n % 5 == 0) {
               m.GotoState("Buzz");
             } else if (n % 3 == 0) {
               m.GotoState("Fizz");
             } else {
               info() << n << '\n' << std::flush;
             }
           }},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "Fizz",
      // Parent State
      "main",
      // On Entry Action
      []() { info() << "Fizz\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{}));

  m.AddState(std::make_unique<State>(
      // State Name
      "Buzz",
      // Parent State
      "main",
      // On Entry Action
      []() { info() << "Buzz\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{}));

  m.AddState(std::make_unique<State>(
      // State Name
      "FizzBuzz",
      // Parent State
      "main",
      // On Entry Action
      []() { info() << "FizzBuzz\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{}));

  m.Start();
}
