#include <sys/time.h>

#include <Wink/log.h>
#include <Wink/socket.h>

UDPSocket::UDPSocket() : sock(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {
  if (sock < 0) {
    error() << "Failed to create socket: " << strerror(errno) << '\n'
            << std::flush;
  }
  auto on = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0) {
    error() << "Failed to set reuse option: " << strerror(errno) << '\n'
            << std::flush;
  }
}

UDPSocket::~UDPSocket() { close(sock); }

int UDPSocket::SetReceiveTimeout(const int seconds) {
  if (sock < 0) {
    return -1;
  }
  struct timeval tv;
  tv.tv_sec = seconds;
  tv.tv_usec = 0;
  return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
}

int UDPSocket::Bind(Address &self) {
  if (sock < 0) {
    return -1;
  }
  struct sockaddr_in address;
  self.writeTo(address);
  socklen_t size = sizeof(struct sockaddr_in);
  if (const auto result = bind(sock, (struct sockaddr *)&address, size);
      result < 0) {
    return result;
  }
  if (const auto result = getsockname(sock, (struct sockaddr *)&address, &size);
      result < 0) {
    return result;
  }
  self.readFrom(address);
  return 0;
}

int UDPSocket::Receive(Address &from, char *buffer, const int length,
                       const int flags) {
  if (sock < 0) {
    return -1;
  }
  struct sockaddr_in address;
  socklen_t size = sizeof(struct sockaddr_in);
  int result =
      recvfrom(sock, buffer, length, flags, (struct sockaddr *)&address, &size);
  if (result < 0) {
    return result;
  }
  from.readFrom(address);
  buffer[result] = 0;
  if (result > 0 && buffer[result - 1] == '\n') {
    buffer[result - 1] = 0;
  }
  return 0;
}

int UDPSocket::Send(const Address &to, const char *buffer, const int length,
                    const int flags) {
  if (sock < 0) {
    return -1;
  }
  struct sockaddr_in address;
  to.writeTo(address);
  socklen_t size = sizeof(struct sockaddr_in);
  return sendto(sock, buffer, std::min(length, (int)MAX_PAYLOAD), flags,
                (struct sockaddr *)&address, size);
}
