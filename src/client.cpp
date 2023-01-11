#include <Wink/client.h>

int StartMachine(Socket &socket, Address &address, const std::string &binary,
                 Address &destination, const std::vector<std::string> args,
                 const bool follow) {
  // Bind to address
  if (const auto result = socket.Bind(address); result < 0) {
    error() << "Failed to bind to address: " << address << " : "
            << std::strerror(errno) << '\n'
            << std::flush;
    return result;
  }

  info() << "Address: " << address << '\n';
  info() << std::flush;

  // Set Receive Timeout
  if (const auto result = socket.SetReceiveTimeout(HEARTBEAT_TIMEOUT);
      result < 0) {
    error() << "Failed to set receive timeout: " << std::strerror(errno) << '\n'
            << std::flush;
    return result;
  }

  // Send Request
  Address server(destination.ip, SERVER_PORT);
  std::ostringstream oss;
  oss << "start ";
  oss << binary;
  oss << " :";
  oss << destination.port;
  for (const auto &a : args) {
    oss << ' ';
    oss << a;
  }
  const auto s = oss.str();
  if (const auto result = SendMessage(socket, server, s); result < 0) {
    return result;
  }

  // Recieve Reply
  char buffer[MAX_PAYLOAD];
  if (const auto result = socket.Receive(destination, buffer, MAX_PAYLOAD);
      result < 0) {
    error() << "Failed to receive packet: " << std::strerror(errno) << '\n'
            << std::flush;
    return result;
  }
  info() << "< " << destination << ' ' << buffer << '\n' << std::flush;
  std::istringstream iss(buffer);
  std::string m;
  iss >> m;
  if (m != "started") {
    error() << "Incorrect message received. Expected: \"started\", Got: \"" << m
            << "\"\n"
            << std::flush;
    return -1;
  }
  std::string b;
  iss >> b;
  if (!b.starts_with(binary)) {
    error() << "Incorrect machine binary started. Expected: \"" << binary
            << "\", Got: \"" << b << "\"\n"
            << std::flush;
    return -1;
  }

  while (follow) {
    if (const auto result = socket.Receive(destination, buffer, MAX_PAYLOAD);
        result < 0) {
      if (errno == EAGAIN) {
        info() << "< " << destination << " errored " << binary
               << " heartbeat timeout\n";
        info() << "< " << destination << " exited " << binary << '\n'
               << std::flush;
        return -1;
      }
      error() << "Failed to receive packet: " << std::strerror(errno) << '\n'
              << std::flush;
      return result;
    }
    info() << "< " << destination << ' ' << buffer << '\n' << std::flush;
    std::istringstream iss(buffer);
    std::string m;
    iss >> m;
    if (m == "exited") {
      break;
    }
  }
  return 0;
}

int StopMachine(Socket &socket, const Address &address) {
  Address server(address.ip, SERVER_PORT);
  std::ostringstream oss;
  oss << "stop ";
  oss << address.port;
  return SendMessage(socket, server, oss.str());
}

int SendMessage(Socket &socket, const Address &address,
                const std::string &message) {
  info() << "> " << address << ' ' << message << '\n' << std::flush;
  if (const auto result =
          socket.Send(address, message.c_str(), message.length() + 1);
      result < 0) {
    error() << "Failed to send packet: " << std::strerror(errno) << '\n'
            << std::flush;
    return result;
  }
  return 0;
}

int ListMachines(Socket &socket, const Address &server) {
  Address address;

  // Bind to address
  if (const auto result = socket.Bind(address); result < 0) {
    error() << "Failed to bind to address: " << address << " : "
            << std::strerror(errno) << '\n'
            << std::flush;
    return result;
  }

  info() << "Address: " << address << '\n';
  info() << std::flush;

  // Set Receive Timeout
  if (const auto result = socket.SetReceiveTimeout(HEARTBEAT_TIMEOUT);
      result < 0) {
    error() << "Failed to set receive timeout: " << std::strerror(errno) << '\n'
            << std::flush;
    return result;
  }

  // Send Request
  if (const auto result = SendMessage(socket, server, "list"); result < 0) {
    return result;
  }

  // Recieve Reply
  Address from;
  char buffer[MAX_PAYLOAD];
  if (const auto result = socket.Receive(from, buffer, MAX_PAYLOAD);
      result < 0) {
    error() << "Failed to receive packet: " << std::strerror(errno) << '\n'
            << std::flush;
    return result;
  }
  info() << "< " << from << ' ' << buffer << '\n' << std::flush;
  return 0;
}
