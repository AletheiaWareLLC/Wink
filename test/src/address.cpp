#include <gtest/gtest.h>

#include <Wink/address.h>

#include <WinkTest/constants.h>

TEST(AddressTest, ReadFrom) {
  struct sockaddr_in a;
  a.sin_family = AF_INET;
  a.sin_addr.s_addr = inet_addr(LOCALHOST);
  a.sin_port = htons(TEST_PORT);
  Address address;
  address.readFrom(a);
  ASSERT_EQ(LOCALHOST, address.ip);
  ASSERT_EQ(TEST_PORT, address.port);
}

TEST(AddressTest, WriteTo) {
  Address address(LOCALHOST, TEST_PORT);
  struct sockaddr_in a;
  address.writeTo(a);
  ASSERT_EQ(AF_INET, a.sin_family);
  ASSERT_EQ(inet_addr(LOCALHOST), a.sin_addr.s_addr);
  ASSERT_EQ(htons(TEST_PORT), a.sin_port);
}

TEST(AddressTest, WriteTo_ResolveHostname) {
  Address address("localhost", TEST_PORT);
  struct sockaddr_in a;
  address.writeTo(a);
  ASSERT_EQ(AF_INET, a.sin_family);
  ASSERT_EQ(inet_addr(LOCALHOST), a.sin_addr.s_addr);
  ASSERT_EQ(htons(TEST_PORT), a.sin_port);
}

TEST(AddressTest, Stream) {
  Address a1(LOCALHOST, TEST_PORT);
  std::ostringstream oss;
  oss << a1;

  Address a2;
  std::istringstream iss(oss.str());
  iss >> a2;

  ASSERT_EQ(LOCALHOST, a2.ip);
  ASSERT_EQ(TEST_PORT, a2.port);
}
