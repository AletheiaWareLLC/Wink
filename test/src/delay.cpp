#include <arpa/inet.h>

#include <gtest/gtest.h>

#include <Wink/delay.h>
#include <Wink/log.h>

TEST(DelayTest, CallAfter) {
  const auto TEST = "test";

  int fd[2]; // [0] for read, [1] for write
  ASSERT_EQ(0, pipe(fd));

  auto f = [&]() {
    close(fd[0]); // Close reading end of pipe
    ASSERT_EQ(5, write(fd[1], TEST, strlen(TEST) + 1)); // Write message
    close(fd[1]); // Close writing end of pipe
  };

  CallAfter(f, 1);

  sleep(2);

  close(fd[1]); // Close writing end of pipe

  char buffer[100];
  ASSERT_EQ(5, read(fd[0], buffer, 100)); // Read message

  close(fd[0]); // Close reading end of pipe

  ASSERT_EQ(strlen(TEST), strlen(buffer));
}
