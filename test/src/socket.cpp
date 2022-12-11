#include <cstring>

#include <Wink/log.h>

#include <WinkTest/constants.h>
#include <WinkTest/socket.h>

int MockSocket::Bind(Address &self) {
  int index = bindArgs.size();
  BindArgs args{self.ip, self.port};
  bindArgs.push_back(args);
  const auto result = bindResults.at(index);
  self.ip = result.selfIP;
  self.port = result.selfPort;
  return result.result;
}

int MockSocket::SetReceiveTimeout(const int seconds) {
  int index = setReceiveTimeoutArgs.size();
  SetReceiveTimeoutArgs args{seconds};
  setReceiveTimeoutArgs.push_back(args);
  return setReceiveTimeoutResults.at(index);
}

int MockSocket::Receive(Address &from, char *buffer, const int length,
                        const int flags) {
  int index = receiveArgs.size();
  ReceiveArgs args;
  args.length = length;
  args.flags = flags;
  receiveArgs.push_back(args);
  const auto result = receiveResults.at(index);
  from.ip = result.fromIP;
  from.port = result.fromPort;
  strcpy(buffer, result.buffer);
  return result.result;
}

int MockSocket::Send(const Address &to, const char *buffer, const int length,
                     const int flags) {
  int index = sendArgs.size();
  SendArgs args;
  args.toIP = to.ip;
  args.toPort = to.port;
  args.length = length;
  args.flags = flags;
  strcpy(args.buffer, buffer);
  sendArgs.push_back(args);
  return sendResults.at(index);
}
