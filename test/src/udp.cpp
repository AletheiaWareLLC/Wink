#include <gtest/gtest.h>

#include <Wink/constants.h>
#include <Wink/socket.h>

TEST(UDPTest, Delivered) {
  UDPSocket socket;
  Address destination(LOCALHOST, 0);
  ASSERT_EQ(0, socket.Bind(destination));

  ASSERT_EQ(0, socket.SetReceiveTimeout(5));

  // Fork child process
  pid_t pid = fork();

  if (pid == 0) {
    // Child sends message after delay
    sleep(1);
    UDPSocket source;
    ASSERT_EQ(5, source.Send(destination, "test", 5));
    _exit(0);
  } else {
    ASSERT_TRUE(pid > 0);
    Address from;
    char buffer[10];
    ASSERT_EQ(0, socket.Receive(from, buffer, 10));
  }
}

TEST(UDPTest, Timeout) {
  UDPSocket socket;
  Address address(LOCALHOST, 0);
  ASSERT_EQ(0, socket.Bind(address));

  ASSERT_EQ(0, socket.SetReceiveTimeout(1));

  Address from;
  char buffer[10];
  ASSERT_EQ(-1, socket.Receive(from, buffer, 10));
  ASSERT_EQ(EAGAIN, errno);
}
