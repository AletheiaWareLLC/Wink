#include <arpa/inet.h>

#include <gtest/gtest.h>

#include <Wink/client.h>
#include <Wink/log.h>

#include <WinkTest/constants.h>
#include <WinkTest/socket.h>

TEST(ClientTest, StartMachine) {
  MockSocket socket;

  // Set mock bind result
  {
    BindResult result;
    result.selfIP = LOCALHOST;
    result.selfPort = TEST_PORT;
    result.result = 0;
    socket.bindResults.push_back(result);
  }
  // Set mock setreceivetimeout result
  {
    SetReceiveTimeoutResult result = 0;
    socket.setReceiveTimeoutResults.push_back(result);
  }
  // Set mock send result
  {
    SendResult result = 0;
    socket.sendResults.push_back(result);
  }
  // Set mock receive result
  {
    ReceiveResult result;
    result.fromIP = TEST_IP;
    result.fromPort = TEST_PORT;
    result.result = 0;
    std::ostringstream oss;
    oss << "started ";
    oss << TEST_BINARY;
    strcpy(result.buffer, oss.str().c_str());
    socket.receiveResults.push_back(result);
  }

  // Issue request
  Address address(LOCALHOST, 0);
  Address destination(TEST_IP, 0);
  std::vector<std::string> args;
  ASSERT_EQ(0, StartMachine(socket, address, TEST_BINARY, destination, args));

  // Check socket receive timeout
  {
    ASSERT_EQ(1, socket.setReceiveTimeoutArgs.size());
    const auto arg = socket.setReceiveTimeoutArgs.at(0);
    ASSERT_EQ(HEARTBEAT_TIMEOUT, arg.seconds);
  }

  // Check socket send
  {
    ASSERT_EQ(1, socket.sendArgs.size());
    const auto arg = socket.sendArgs.at(0);
    ASSERT_EQ(TEST_IP, arg.toIP);
    ASSERT_EQ(SERVER_PORT, arg.toPort);
    ASSERT_EQ(std::string("start wink.bin :0"), std::string(arg.buffer));
    ASSERT_EQ(18, arg.length);
  }

  // Check destination address
  ASSERT_EQ(TEST_IP, destination.ip);
  ASSERT_EQ(TEST_PORT, destination.port);
}

TEST(ClientTest, StopMachine) {
  MockSocket socket;

  // Set mock send result
  {
    SendResult result = 0;
    socket.sendResults.push_back(result);
  }

  Address address(TEST_IP, TEST_PORT);
  ASSERT_EQ(0, StopMachine(socket, address));

  // Check socket send
  ASSERT_EQ(1, socket.sendArgs.size());
  const auto arg = socket.sendArgs.at(0);
  ASSERT_EQ(TEST_IP, arg.toIP);
  ASSERT_EQ(SERVER_PORT, arg.toPort);
  ASSERT_EQ(std::string("stop 42424"), std::string(arg.buffer));
  ASSERT_EQ(11, arg.length);
}

TEST(ClientTest, SendMessage) {
  MockSocket socket;

  // Set mock send result
  {
    SendResult result = 0;
    socket.sendResults.push_back(result);
  }

  Address address(TEST_IP, TEST_PORT);
  ASSERT_EQ(0, SendMessage(socket, address, TEST_MESSAGE));

  // Check socket send
  ASSERT_EQ(1, socket.sendArgs.size());
  const auto arg = socket.sendArgs.at(0);
  ASSERT_EQ(TEST_IP, arg.toIP);
  ASSERT_EQ(TEST_PORT, arg.toPort);
  ASSERT_EQ(TEST_MESSAGE, std::string(arg.buffer));
  ASSERT_EQ(10, arg.length);
}
