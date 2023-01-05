#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <arpa/inet.h>

#include <Wink/constants.h>
#include <Wink/log.h>
#include <Wink/machine.h>
#include <Wink/socket.h>

int StartMachine(Socket &socket, Address &address, const std::string &binary,
                 Address &destination, const std::vector<std::string> args,
                 const bool follow = false);
int StopMachine(Socket &socket, const Address &address);
int SendMessage(Socket &socket, const Address &address,
                const std::string &message);
int ListMachines(Socket &socket, const Address &server);

#endif
