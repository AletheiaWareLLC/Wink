#include <Wink/machine.h>

void Machine::Start(const std::string &initial) {
  if (states.empty()) {
    // Nothing to do
    return;
  }

  // Bind to port
  if (const auto result = socket.Bind(address); result < 0) {
    error() << "Failed to bind to port " << address.port << '\n' << std::flush;
    return;
  }

  std::ostringstream oss;
  oss << name;
  oss << '@';
  oss << address;
  uid = oss.str();

  // Set Receive Timeout
  if (const auto result = socket.SetReceiveTimeout(REPLY_TIMEOUT); result < 0) {
    error() << "Failed to set receive timeout: " << std::strerror(errno) << '\n'
            << std::flush;
    return;
  }

  info() << uid << " started\n" << std::flush;

  // Notify parent of start
  Send(spawner, "started " + name);

  // Register with server
  registerMachine(name, getpid());

  std::string state = current;
  if (!initial.empty()) {
    state = initial;
  }
  current = "";
  GotoState(state);

  // Loop receiving messages
  auto last = std::chrono::system_clock::now();
  while (running) {
    const auto now = std::chrono::system_clock::now();
    checkHealthOfSpawned(now); // Check every loop
    if (now - last > std::chrono::seconds(PULSE_INTERVAL)) {
      sendPulseToSpawner(); // Send every PULSE_INTERVAL
      last = now;
    }
    sendScheduled(now);  // Send any scheduled messages
    receiveMessage(now); // Waits up to REPLY_TIMEOUT for message
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
  Send(spawner, oss.str());

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
  Send(spawner, oss.str());

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
    error() << uid << ": Failed to send packet: " << std::strerror(errno)
            << '\n'
            << std::flush;
  }
}

void Machine::SendAt(
    const Address &address, const std::string &message,
    const std::chrono::time_point<std::chrono::system_clock> time) {
  queue.push_back(ScheduledMessage{address, message, time});
}

void Machine::SendAfter(const Address &address, const std::string &message,
                        const std::chrono::seconds delay) {
  auto time = std::chrono::system_clock::now();
  time += delay;
  SendAt(address, message, time);
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

void Machine::checkHealthOfSpawned(
    const std::chrono::time_point<std::chrono::system_clock> now) {
  std::vector<std::string> dead;
  for (const auto &[k, v] : spawned) {
    if (const auto d = now - v.second;
        d > std::chrono::seconds(HEARTBEAT_TIMEOUT)) {
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

void Machine::sendPulseToSpawner() { Send(spawner, "pulsed " + name); }

void Machine::sendScheduled(
    const std::chrono::time_point<std::chrono::system_clock> now) {
  std::vector<ScheduledMessage> q;
  q.swap(queue);
  for (const auto &e : q) {
    if (e.time < now) {
      Send(e.address, e.message);
    } else {
      queue.push_back(e);
    }
  }
}

void Machine::receiveMessage(
    const std::chrono::time_point<std::chrono::system_clock> now) {
  if (const auto result = socket.Receive(sender, buffer, MAX_PAYLOAD);
      result < 0) {
    if (errno == EAGAIN) {
      return;
    }
    error() << uid << ": Failed to receive packet: " << std::strerror(errno)
            << '\n'
            << std::flush;

    return;
  }
  handleMessage(now);
}

void Machine::handleMessage(
    const std::chrono::time_point<std::chrono::system_clock> now) {
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
