#ifndef MACHINE_H
#define MACHINE_H

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

#include <Wink/address.h>
#include <Wink/client.h>
#include <Wink/log.h>
#include <Wink/socket.h>
#include <Wink/state.h>

class Machine {
public:
  Machine(Address &spawner, Address &address, std::string name, Socket &socket)
      : spawner(spawner), address(address), name(name), socket(socket) {}
  Machine(const Machine &m) = delete;
  Machine(Machine &&m) = delete;
  ~Machine() {}
  /**
   * Returns this Machine's Unique Identifier.
   */
  std::string Uid() const { return uid; }
  /**
   * Start begins execution of the state machine and sends a
   * 'started' message to the machine which spawned this machine.
   *
   * Parameter initial sets the state in which the state machine
   * will start. If empty, the state machine will start in the
   * first state that was added.
   */
  void Start(const std::string &initial = "");
  /**
   * Exit immediately ceases execution of the state machine and sends an
   * 'exited' message to the machine which spawned this machine.
   */
  void Exit();
  /**
   * Error immediately ceases execution of the state machine and sends an
   * 'errored' message and the given error message to the machine which
   * spawned this machine.
   */
  void Error(const std::string &message);
  /**
   * AddState adds the given state to this state machine.
   */
  void AddState(std::unique_ptr<State> &&state);
  /**
   * GotoState transitions the state machine to the given state.
   */
  void GotoState(const std::string &state);
  /**
   * Send transmits a message to the given address.
   */
  void Send(const Address &address, const std::string &message);
  /**
   * SendAt sends the given address the message at the given time.
   */
  void SendAt(const Address &address, const std::string &message,
              const std::chrono::time_point<std::chrono::system_clock> time);
  /**
   * SendAfter sends the given address the message after the given delay.
   */
  void SendAfter(const Address &address, const std::string &message,
                 const std::chrono::seconds delay);
  /**
   * Spawn spawns a new state machine.
   */
  void Spawn(const std::string &machine);
  /**
   * Spawn spawns a new state machine with the given arguments.
   */
  void Spawn(const std::string &machine, const std::vector<std::string> &args);
  /**
   * Spawn spawns a new state machine at the given address.
   */
  void Spawn(const std::string &machine, const Address &address);
  /**
   * Spawn spawns a new state machine at the given address with the given
   * arguments.
   */
  void Spawn(const std::string &machine, const Address &address,
             const std::vector<std::string> &args);

  std::function<void()> onExit = []() { _exit(0); };

private:
  void checkHealthOfSpawned(
      const std::chrono::time_point<std::chrono::system_clock> now);
  void sendPulseToSpawner();
  void
  sendScheduled(const std::chrono::time_point<std::chrono::system_clock> now);
  void
  receiveMessage(const std::chrono::time_point<std::chrono::system_clock> now);
  void
  handleMessage(const std::chrono::time_point<std::chrono::system_clock> now);
  void registerMachine(const std::string &machine, const int pid);
  void unregisterMachine();

  Address &spawner;
  Address &address;
  std::string name;
  std::string uid;
  bool running = true;
  Socket &socket;
  Address sender;
  char buffer[MAX_PAYLOAD];
  std::map<const std::string, const std::unique_ptr<State>> states;
  std::string current;
  struct ScheduledMessage {
    const Address &address;
    const std::string message;
    const std::chrono::time_point<std::chrono::system_clock> time;
  };
  std::vector<ScheduledMessage> queue;
  std::map<const std::string,
           std::pair<const std::string,
                     std::chrono::time_point<std::chrono::system_clock>>>
      spawned;
};

#endif
