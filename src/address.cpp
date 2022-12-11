#include <cctype>
#include <cstring>

#include <Wink/address.h>

void Address::stoa(const std::string &address) {
  const auto index = address.find(':');
  switch (index) {
  case std::string::npos:
    ip = address;
    port = 0;
    break;
  case 0:
    ip = std::string(LOCALHOST);
    port = std::stoul(address.substr(index + 1));
    break;
  default:
    ip = address.substr(0, index);
    port = std::stoul(address.substr(index + 1));
    break;
  }
}

std::string Address::atos() const {
  std::ostringstream oss;
  oss << ip;
  oss << ':';
  oss << port;
  return oss.str();
}

void Address::readFrom(const struct sockaddr_in &address) {
  ip = std::string(inet_ntoa(address.sin_addr));
  port = ntohs(address.sin_port);
}

void Address::writeTo(struct sockaddr_in &address) const {
  memset(&address, 0, sizeof(struct sockaddr_in));
  address.sin_family = AF_INET;
  if (!ip.empty()) {
    auto c = ip.c_str();
    if (!isdigit(c[0])) {
      if (const auto record = gethostbyname(c); record) {
        in_addr *address = (in_addr *)record->h_addr;
        c = inet_ntoa(*address);
      }
    }
    address.sin_addr.s_addr = inet_addr(c);
  }
  address.sin_port = htons(port);
}

std::istream &operator>>(std::istream &is, Address &a) {
  std::string s;
  is >> s;
  a.stoa(s);
  return is;
}

std::ostream &operator<<(std::ostream &os, const Address &a) {
  os << a.ip << ':' << a.port;
  return os;
}
