#include <Wink/machine.h>

void Machine::Start(const std::string &initial) {
  // Bind to port
  if (const auto result = socket.Bind(address); result < 0) {
    error() << "Failed to bind to port " << address.port << '\n' << std::flush;
    return;
  }

  std::ostringstream oss;
  oss << address;
  oss << ' ';
  oss << name;
  uid = oss.str();

  // Set Receive Timeout
  if (const auto result = socket.SetReceiveTimeout(REPLY_TIMEOUT); result < 0) {
    error() << "Failed to set receive timeout: " << strerror(errno) << '\n'
            << std::flush;
    return;
  }

  info() << uid << " started\n" << std::flush;

  // Notify parent of start
  SendSpawner("started " + name);

  // Register with server
  registerMachine(name, getpid());

  std::string state = current;
  if (!initial.empty()) {
    state = initial;
  }
  current = "";
  GotoState(state);

  // Loop receiving messages
  std::time_t last = std::time(nullptr);
  while (running) {
    const std::time_t now = std::time(nullptr);
    if (now - last > PULSE_INTERVAL) {
      sendPulseToSpawner(); // Send every PULSE_INTERVAL
      last = now;
    }
    checkHealthOfSpawned(now); // Check every loop
    receiveMessage(now);       // Waits up to REPLY_TIMEOUT for message
  }
}

void Machine::Exit() {
  info() << uid << " exited\n" << std::flush;

  // Exit current state
  if (const auto it = states.find(current); it != states.end()) {
    it->second->onExit();
  }

  std::ostringstream oss;
  oss << "exited ";
  oss << name;

  // Notify parent of exit
  SendSpawner(oss.str());

  // Unregister with server
  unregisterMachine();

  // TODO kill all child machines

  onExit();
  running = false;
  return;
}

void Machine::Error(const std::string &message) {
  info() << uid << " errored: " << message << '\n' << std::flush;

  std::ostringstream oss;
  oss << "errored ";
  oss << name;
  oss << ' ';
  oss << message;

  // Notify parent of error
  SendSpawner(oss.str());

  Exit();
  return;
}

void Machine::AddState(std::unique_ptr<State> &&state) {
  const auto n = state->name;
  if (current.empty()) {
    current = n;
  }
  states.emplace(n, std::move(state));
}

void Machine::GotoState(const std::string &state) {
  // Exit current state
  if (const auto it = states.find(current); it != states.end()) {
    it->second->onExit();
  }
  current = state;
  // Enter new state
  if (const auto it = states.find(current); it != states.end()) {
    it->second->onEnter();
  }
}

void Machine::Send(const Address &address, const std::string &message) {
  info() << uid << " > " << address << ' ' << message << '\n' << std::flush;
  if (const auto result =
          socket.Send(address, message.c_str(), message.length() + 1);
      result < 0) {
    error() << uid << ": Failed to send packet: " << strerror(errno) << '\n'
            << std::flush;
  }
}

void Machine::SendSelf(const std::string &message) { Send(address, message); }

void Machine::SendSpawner(const std::string &message) {
  Send(spawner, message);
}

void Machine::Spawn(const std::string &machine) {
  std::vector<std::string> args;
  Spawn(machine, args);
}

void Machine::Spawn(const std::string &machine,
                    const std::vector<std::string> &args) {
  const Address destination(address.ip, 0);
  Spawn(machine, destination, args);
}

void Machine::Spawn(const std::string &machine, const Address &address) {
  std::vector<std::string> args;
  Spawn(machine, address, args);
}

void Machine::Spawn(const std::string &machine, const Address &address,
                    const std::vector<std::string> &args) {
  // Send Request
  Address server(address.ip, SERVER_PORT);
  std::ostringstream oss;
  oss << "start ";
  oss << machine;
  oss << " :";
  oss << address.port;
  for (const auto &a : args) {
    oss << ' ';
    oss << a;
  }
  const auto s = oss.str();
  Send(server, s);
}

void Machine::sendPulseToSpawner() { SendSpawner("pulsed " + name); }

void Machine::checkHealthOfSpawned(const std::time_t now) {
  std::vector<std::string> dead;
  for (const auto &[k, v] : spawned) {
    if (const auto d = now - v.second; d > HEARTBEAT_TIMEOUT) {
      dead.push_back(k);
    }
  }

  for (const auto &c : dead) {
    if (const auto &it = spawned.find(c); it != spawned.end()) {
      const auto k = it->first;
      const auto v = it->second;

      std::istringstream iss(k);
      iss >> sender;

      {
        // Send errored message
        std::ostringstream oss;
        oss << "errored ";
        oss << v.first;
        oss << " heartbeat timeout";
        const auto s = oss.str();
        s.copy(buffer, s.size());
        buffer[s.size()] = 0;
        handleMessage(now);
      }

      {
        // Send exited message
        std::ostringstream oss;
        oss << "exited ";
        oss << v.first;
        const auto s = oss.str();
        s.copy(buffer, s.size());
        buffer[s.size()] = 0;
        handleMessage(now);
      }
    }
  }
}

void Machine::receiveMessage(const std::time_t now) {
  if (const auto result = socket.Receive(sender, buffer, MAX_PAYLOAD);
      result < 0) {
    if (errno == EAGAIN) {
      return;
    }
    error() << uid << ": Failed to receive packet: " << strerror(errno) << '\n'
            << std::flush;

    return;
  }
  handleMessage(now);
}

void Machine::handleMessage(const std::time_t now) {
  info() << uid << " < " << sender << ' ' << buffer << '\n' << std::flush;
  std::istringstream iss(buffer);
  std::string m;
  iss >> m;

  // Supervision
  std::ostringstream oss;
  oss << sender;
  const auto key = oss.str();
  if (m == "started") {
    // Start tracking spawned health
    std::string name;
    iss >> name;
    spawned.emplace(key, std::make_pair(name, now));
  } else if (m == "exited") {
    spawned.erase(key);
  } else if (m == "pulsed") {
    if (auto it = spawned.find(key); it != spawned.end()) {
      it->second.second = now;
    }
  }

  auto s = current;
  while (!s.empty()) {
    if (const auto it = states.find(s); it != states.end()) {
      const auto rs = it->second->receivers;
      if (const auto i = rs.find(m); i != rs.end()) {
        i->second(sender, iss);
        return;
      } else if (const auto i = rs.find(""); i != rs.end()) {
        std::istringstream iss(buffer);
        i->second(sender, iss);
        return;
      } else {
        // Message not handled by state, try parent
        s = it->second->parent;
      }
    } else {
      error() << uid << ": No such state: " << s << '\n' << std::flush;
      Error("unrecognized state: " + s);
    }
  }
  // Message not handled by hierarchy
  error() << uid << ": Failed to handle message\n" << std::flush;
  Error("unhandled message: " + m);
}

void Machine::registerMachine(const std::string &machine, const int pid) {
  std::ostringstream oss;
  oss << "register ";
  oss << machine;
  oss << ' ';
  oss << pid;
  Address server(address.ip, SERVER_PORT);
  Send(server, oss.str());
}

void Machine::unregisterMachine() {
  Address server(address.ip, SERVER_PORT);
  Send(server, "unregister");
}
