#include <Wink/server/server.h>

int Server::Serve(const std::string &directory) {
  if (!log.empty()) {
    if (const auto result = logToFile(log, "server"); result < 0) {
      error() << "Failed to setup logging\n" << std::flush;
      return -1;
    }
  }

  info() << "Directory: " << directory << '\n';

  // Bind to address
  if (const auto result = socket.Bind(address); result < 0) {
    error() << "Failed to bind to address: " << address << " : "
            << strerror(errno) << '\n'
            << std::flush;
    return result;
  }

  info() << "Address: " << address << '\n';
  info() << std::flush;

  // Set Receive Timeout
  if (const auto result = socket.SetReceiveTimeout(NO_TIMEOUT); result < 0) {
    error() << "Failed to set receive timeout: " << strerror(errno) << '\n'
            << std::flush;
    return result;
  }

  char buffer[MAX_PAYLOAD];
  Address sender;
  while (1) {
    if (const auto result = socket.Receive(sender, buffer, MAX_PAYLOAD);
        result < 0) {
      error() << "Failed to receive packet: " << strerror(errno) << '\n'
              << std::flush;
      return -1;
    }
    info() << "< " << sender << ' ' << buffer << '\n';
    info() << std::flush;

    std::istringstream iss(buffer);
    std::string command;
    iss >> command;
    if (command == "start") {
      std::string machine;
      iss >> machine;

      // start <machine>
      Address destination;
      // start <machine> <port>
      if (iss.good()) {
        iss >> destination;
      }
      destination.ip = address.ip;

      if (const auto port = destination.port; port > 0) {
        // Stop existing machine on requested port (if any)
        Stop(port);
      }

      std::vector<std::string> parameters;

      std::stringstream bs;
      bs << directory;
      bs << '/';
      bs << machine;
      const auto b = bs.str();
      parameters.push_back(b);

      std::stringstream ss;
      ss << sender;
      const auto s = ss.str();
      parameters.push_back(s);

      std::stringstream as;
      as << destination;
      const auto a = as.str();
      parameters.push_back(a);

      while (iss.good()) {
        std::string p;
        iss >> p;
        parameters.push_back(p);
      }

      if (const auto result = Start(machine, parameters); result < 0) {
        error() << "Failed to start process\n" << std::flush;
        return result;
      }
    } else if (command == "stop") {
      ushort port;
      iss >> port;
      if (const auto result = Stop(port); result < 0) {
        error() << "Failed to stop process\n" << std::flush;
        return result;
      }
    } else if (command == "register") {
      std::string machine;
      iss >> machine;
      int pid;
      iss >> pid;
      machines.insert(std::pair<ushort, std::string>(sender.port, machine));
      pids.insert(std::pair<ushort, int>(sender.port, pid));
    } else if (command == "unregister") {
      if (const auto it = pids.find(sender.port); it != pids.end()) {
        // remove port from machines and pids maps
        machines.erase(sender.port);
        pids.erase(sender.port);
      } else {
        error() << "Unrecognized port " << sender.port << '\n' << std::flush;
      }
    } else if (command == "list") {
      // List Machines
      std::ostringstream oss;
      oss << "Port,Machine,PID\n";
      for (const auto &[k, v] : machines) {
        oss << k << ',' << v << ',' << pids[k] << '\n';
      }
      const auto s = oss.str();
      if (const auto result = SendMessage(socket, sender, s); result < 0) {
        return result;
      }
    } else {
      error() << "Failed to parse " << buffer << '\n' << std::flush;
    }
  }

  return 0;
}

int Server::Start(const std::string &machine,
                  const std::vector<std::string> &parameters) {
  // Fork child process
  pid_t pid;
  switch (pid = fork()) {
  case -1:
    error() << "Failed to fork process: " << strerror(errno) << '\n'
            << std::flush;
    return -1;
  case 0: {
    // Child runs machine process
    if (!log.empty()) {
      std::string s(machine);
      std::replace(s.begin(), s.end(), '/', '_');
      if (const auto result = logToFile(log, s); result < 0) {
        error() << "Failed to setup logging\n" << std::flush;
        return -1;
      }
    }

    const auto length = parameters.size();
    const auto args = new char *[length + 1];
    args[length] = NULL;

    uint i = 0;
    for (const auto &p : parameters) {
      const auto l = p.length() + 1;
      const auto c = p.c_str();
      auto a = new char[l];
      strncpy(a, c, l);
      args[i] = a;
      i++;
    }

    const auto result = execv(args[0], args);

    for (i = 0; i < length; i++) {
      delete[] args[i];
    }
    delete[] args;

    if (result < 0) {
      error() << "Failed to execute binary: " << parameters.at(0) << ": "
              << strerror(errno) << '\n'
              << std::flush;
      return result;
    }
    _exit(0);
  }
  default:
    // Parent does nothing
    info() << "Forked: " << pid << '\n' << std::flush;
  }
  return 0;
}

int Server::Stop(int port) {

  int pid = 0;
  if (const auto it = pids.find(port); it != pids.end()) {
    pid = it->second;
  }
  if (pid > 0) {
    if (const auto result = kill(pid, SIGTERM); result < 0) {
      return result;
    }
    // remove port from machines and pids maps
    machines.erase(port);
    pids.erase(port);
  } else {
    error() << "Unrecognized port " << port << '\n' << std::flush;
  }
  return 0;
}
