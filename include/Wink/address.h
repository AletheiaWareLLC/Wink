#ifndef ADDRESS_H
#define ADDRESS_H

#include <iostream>
#include <netdb.h>
#include <sstream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <Wink/constants.h>
#include <Wink/log.h>

class Address {
public:
  std::string ip;
  ushort port;

  Address() : ip(LOCALHOST), port(0) {}
  Address(std::string address) { stoa(address); }
  Address(std::string ip, ushort port) : ip(ip), port(port) {}
  Address(const Address &a) = delete;
  Address(Address &&a) = delete;
  ~Address() {}
  void stoa(const std::string &address);
  std::string atos() const;
  void readFrom(const struct sockaddr_in &address);
  void writeTo(struct sockaddr_in &address) const;
};

std::istream &operator>>(std::istream &is, Address &a);
std::ostream &operator<<(std::ostream &os, const Address &a);

#endif
