# Wink

Wink is framework for developing Fault Tolerant Systems with Asynchronous Concurrent Independent Hierarchical Finite State Machine.

## Finite State Machine

A Finite State Machine;
- Has a Hierarchy of States to minimize Code Duplication.
- Runs Independently and Concurrently for Fault Isolation.
- Communicates Asynchronously to minimize Latency and maximize Throughput.
- Monitors Lifecycle with a Supervision Tree.
- Is uniquely identified by its Binary Name and Network Address;
  - local `:<port>` or `localhost:<port>` or `127.0.0.1:<port>`
  - remote `<ip>:<port>`

### States

Each State consists of a unique Name, an optional Parent, an optional Entry Action, an optional Exit Action, and a set of Receivers.

### Actions

An action is triggered when a state is entered or exited.

### Receivers

A receiver is triggered upon receipt of a matching message.

The optional empty receiver is triggered if no other receivers match.

## Example

```
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
  Machine m(spawner, address, "switch/Switch", socket);

  m.AddState(std::make_unique<State>(
      // State Name
      "off",
      // Parent State
      "",
      // On Entry Action
      []() { info() << "Switch is OFF\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"on", [&](const Address &sender,
                     std::istream &args) { m.GotoState("on"); }},
          {"off", [&](const Address &sender, std::istream &args) {}},
      }));

  m.AddState(std::make_unique<State>(
      // State Name
      "on",
      // Parent State
      "",
      // On Entry Action
      []() { info() << "Switch is ON\n"
                    << std::flush; },
      // On Exit Action
      []() {},
      // Receivers
      std::map<const std::string, Receiver>{
          {"on", [&](const Address &sender, std::istream &args) {}},
          {"off", [&](const Address &sender,
                      std::istream &args) { m.GotoState("off"); }},
      }));

  m.Start();
}
```

## Repository

 - include: header files
 - samples: code samples
 - src: source code files
 - test: test code files

## Build

```cmake
cmake -S . -B build
cmake --build build
```

## Test

```
(cd build/test/src && ctest)
(cd build/test/samples && ctest)
```

## Docker

```
docker build . -t wink:latest
```

# Usage

## Client
```
./build/src/Wink
```

### Start

Starts a new machine from a binary.

```
./build/src/Wink start [options] <binary>
./build/src/Wink start [options] <binary> :<port>
./build/src/Wink start [options] <binary> <ip>:<port>
```

### Stop

Stops an existing machine.

```
./build/src/Wink stop [options] :<port>
./build/src/Wink stop [options] <ip>:<port>
```

### Send

Sends a message to a machine.

```
./build/src/Wink send [options] :<port> <message>
./build/src/Wink send [options] <ip>:<port> <message>
```

### List

List existing machines running on a server.

```
./build/src/Wink list [options]
./build/src/Wink list [options] :<port>
./build/src/Wink list [options] <ip>:<port>
```

### Help

```
./build/src/Wink help
./build/src/Wink help <command>
```

## Server

Starts the WinkServer serving from the given directory.

```
./build/src/WinkServer serve <directory>
```

### Help

```
./build/src/WinkServer help
./build/src/WinkServer help <command>
```

---

Inspired by @winksaville
