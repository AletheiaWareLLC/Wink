#ifndef SERVER_H
#define SERVER_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <signal.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <Wink/address.h>
#include <Wink/client.h>
#include <Wink/constants.h>
#include <Wink/log.h>
#include <Wink/machine.h>
#include <Wink/semver.h>
#include <Wink/socket.h>

class Server {
public:
  explicit Server(Address &address, Socket &socket, const std::string &log = "")
      : address(address), socket(socket), log(log) {}
  Server(const Server &m) = delete;
  Server(Server &&m) = delete;
  ~Server() {}
  int Serve(const std::string &directory);
  int Start(const std::string &binary, const std::string &machine,
            const std::vector<std::string> &parameters);
  int Stop(int port);

private:
  Address &address;
  Socket &socket;
  const std::string &log;
  // Map port number to machine file
  std::map<ushort, std::string> machines;
  // Map port number to process identifier
  std::map<ushort, int> pids;
};

std::string resolveVersion(const std::string &directory,
                           const std::string &machine);

#endif
