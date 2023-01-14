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

  m.AddState(std::make_unique<State>(
      "Leaf1", "", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Leaf1: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Parent1", "", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Parent1: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Leaf2", "Parent1", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Leaf2: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Child2", "Parent1",
      []() { info() << "Child2 On Entry\n"
                    << std::flush; }, []() {},
      std::map<const std::string, Receiver>{
          {"Test",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Child2: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Leaf4", "Child2", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Leaf4: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Leaf5", "Child2", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Leaf5: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Parent2", "", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Parent2: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Child3", "Parent2",
      []() { info() << "Child3 On Entry\n"
                    << std::flush; }, []() {},
      std::map<const std::string, Receiver>{
          {"Test",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Child3: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Leaf6", "Child3", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Leaf6: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Child4", "Parent2",
      []() { info() << "Child4 On Entry\n"
                    << std::flush; }, []() {},
      std::map<const std::string, Receiver>{
          {"Test",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Child4: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Leaf7", "Child4", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Leaf7: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Parent3", "", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Parent3: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.AddState(std::make_unique<State>(
      "Leaf3", "Parent3", []() {}, []() {},
      std::map<const std::string, Receiver>{
          {"",
           [&](const Address &sender, std::istream &args) {
             std::ostringstream os;
             os << args.rdbuf();
             info() << "Leaf3: " << os.str() << '\n' << std::flush;
           }},
      }));

  m.Start();
}
