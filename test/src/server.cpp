#include <gtest/gtest.h>

#include <Wink/address.h>
#include <Wink/server/server.h>

#include <WinkTest/constants.h>
#include <WinkTest/socket.h>

TEST(ServerTest, Serve) {
  // TODO
  // Bind to Address
  // Set Receive Timeout
  // Repeatedly
  // - Receive Message
  // - Parse as (start,stop,register,unregister,list)
}

TEST(ServerTest, Start) {
  Address address(LOCALHOST, SERVER_PORT);
  UDPSocket socket;
  Server server(address, socket);
  const std::vector<std::string> args;
  ASSERT_EQ(0, server.Start("./samples/", "useless/Useless", args));
}

TEST(ServerTest, Stop) {
  // TODO
}
