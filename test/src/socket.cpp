#include <cstring>

#include <Wink/log.h>

#include <WinkTest/constants.h>
#include <WinkTest/socket.h>

int MockSocket::Bind(Address &self) {
  const auto index = bindArgs.size();
  BindArgs args{self.ip, self.port};
  bindArgs.push_back(args);
  if (index >= bindResults.size()) {
    error() << "Unexpected call to Bind\n" << std::flush;
  }
  const auto result = bindResults.at(index);
  self.ip = result.selfIP;
  self.port = result.selfPort;
  return result.result;
}

int MockSocket::SetReceiveTimeout(const int seconds) {
  const auto index = setReceiveTimeoutArgs.size();
  SetReceiveTimeoutArgs args{seconds};
  setReceiveTimeoutArgs.push_back(args);
  if (index >= setReceiveTimeoutResults.size()) {
    error() << "Unexpected call to SetReceiveTimeout\n" << std::flush;
  }
  return setReceiveTimeoutResults.at(index);
}

int MockSocket::Receive(Address &from, char *buffer, const int length,
                        const int flags) {
  const auto index = receiveArgs.size();
  ReceiveArgs args;
  args.length = length;
  args.flags = flags;
  receiveArgs.push_back(args);
  if (index >= receiveResults.size()) {
    error() << "Unexpected call to Receive\n" << std::flush;
  }
  const auto result = receiveResults.at(index);
  from.ip = result.fromIP;
  from.port = result.fromPort;
  strcpy(buffer, result.buffer);
  return result.result;
}

int MockSocket::Send(const Address &to, const char *buffer, const int length,
                     const int flags) {
  const auto index = sendArgs.size();
  SendArgs args;
  args.toIP = to.ip;
  args.toPort = to.port;
  args.length = length;
  args.flags = flags;
  strcpy(args.buffer, buffer);
  sendArgs.push_back(args);
  if (index >= sendResults.size()) {
    error() << "Unexpected call to Send\n" << std::flush;
  }
  return sendResults.at(index);
}
