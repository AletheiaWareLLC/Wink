#ifndef SOCKET_TEST_H
#define SOCKET_TEST_H

#include <algorithm>
#include <cstring>
#include <vector>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <Wink/address.h>
#include <Wink/constants.h>
#include <Wink/socket.h>

struct BindArgs {
  std::string selfIP;
  ushort selfPort;
};

struct BindResult {
  std::string selfIP;
  ushort selfPort;
  int result;
};

struct SetReceiveTimeoutArgs {
  int seconds;
};

typedef int SetReceiveTimeoutResult;

struct ReceiveArgs {
  int length;
  int flags;
};

struct ReceiveResult {
  std::string fromIP;
  ushort fromPort;
  char buffer[MAX_PAYLOAD];
  int result;
};

struct SendArgs {
  std::string toIP;
  ushort toPort;
  char buffer[MAX_PAYLOAD];
  int length;
  int flags;
};

typedef int SendResult;

class MockSocket : public Socket {
public:
  MockSocket() {}
  MockSocket(const MockSocket &s) = delete;
  MockSocket(MockSocket &&s) = delete;
  ~MockSocket() {}
  int Bind(Address &self) override;
  int SetReceiveTimeout(const int seconds) override;
  int Receive(Address &from, char *buffer, const int length,
              const int flags = 0) override;
  int Send(const Address &to, const char *buffer, const int length,
           const int flags = 0) override;
  std::vector<BindArgs> bindArgs;
  std::vector<BindResult> bindResults;
  std::vector<SetReceiveTimeoutArgs> setReceiveTimeoutArgs;
  std::vector<SetReceiveTimeoutResult> setReceiveTimeoutResults;
  std::vector<ReceiveArgs> receiveArgs;
  std::vector<ReceiveResult> receiveResults;
  std::vector<SendArgs> sendArgs;
  std::vector<SendResult> sendResults;
};

#endif
