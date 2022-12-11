#include <gtest/gtest.h>

#include <Wink/machine.h>

#include <WinkTest/constants.h>
#include <WinkTest/socket.h>

TEST(MachineTest, Start) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;
  // Set mock bind result
  {
    BindResult result;
    result.selfIP = LOCALHOST;
    result.selfPort = 42002;
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
    socket.sendResults.push_back(result);
    socket.sendResults.push_back(result);
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

  int mainOnEntry;
  int mainOnExit;

  Machine m(spawner, address, name, socket);
  // Override exit
  m.onExit = []() {};
  m.AddState(std::make_unique<State>(
      // State Name
      "main",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        mainOnEntry++;
        m.Exit();
      },
      // On Exit Action
      [&]() { mainOnExit++; },
      // Receivers
      std::map<const std::string, Receiver>{}));
  m.Start();

  ASSERT_EQ("127.0.0.1:42002 test/Test", m.Uid());

  // Bind call
  { ASSERT_EQ(1, socket.bindArgs.size()); }

  // Check socket receive timeout
  {
    ASSERT_EQ(1, socket.setReceiveTimeoutArgs.size());
    const auto arg = socket.setReceiveTimeoutArgs.at(0);
    ASSERT_EQ(REPLY_TIMEOUT, arg.seconds);
  }

  // Check socket send
  {
    ASSERT_EQ(4, socket.sendArgs.size());
    // Send Started Message to Spawner
    {
      const auto arg0 = socket.sendArgs.at(0);
      ASSERT_EQ(spawner.ip, arg0.toIP);
      ASSERT_EQ(spawner.port, arg0.toPort);
      ASSERT_EQ(std::string("started test/Test"), std::string(arg0.buffer));
      ASSERT_EQ(18, arg0.length);
    }
    // Register Machine
    {
      const auto arg1 = socket.sendArgs.at(1);
      ASSERT_EQ(LOCALHOST, arg1.toIP);
      ASSERT_EQ(SERVER_PORT, arg1.toPort);
      std::string message(arg1.buffer);
      ASSERT_TRUE(message.starts_with("register test/Test "));
    }
    // Send Exited Message to Spawner
    {
      const auto arg2 = socket.sendArgs.at(2);
      ASSERT_EQ(spawner.ip, arg2.toIP);
      ASSERT_EQ(spawner.port, arg2.toPort);
      ASSERT_EQ(std::string("exited test/Test"), std::string(arg2.buffer));
      ASSERT_EQ(17, arg2.length);
    }
    // Unregister Machine
    {
      const auto arg3 = socket.sendArgs.at(3);
      ASSERT_EQ(LOCALHOST, arg3.toIP);
      ASSERT_EQ(SERVER_PORT, arg3.toPort);
      ASSERT_EQ(std::string("unregister"), std::string(arg3.buffer));
      ASSERT_EQ(11, arg3.length);
    }
  }
  // Initial State Transitions
  ASSERT_EQ(1, mainOnEntry);
  ASSERT_EQ(1, mainOnExit);
}

TEST(MachineTest, Start_InitialState) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;
  // Set mock bind result
  {
    BindResult result;
    result.selfIP = LOCALHOST;
    result.selfPort = 42002;
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
    socket.sendResults.push_back(result);
    socket.sendResults.push_back(result);
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

  int firstOnEntry;
  int firstOnExit;
  int secondOnEntry;
  int secondOnExit;

  Machine m(spawner, address, name, socket);
  // Override exit
  m.onExit = []() {};
  m.AddState(std::make_unique<State>(
      // State Name
      "first",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        firstOnEntry++;
        info() << "firstOnEntry: " << firstOnEntry << '\n' << std::flush;
        m.Exit();
      },
      // On Exit Action
      [&]() {
        firstOnExit++;
        info() << "firstOnExit: " << firstOnExit << '\n' << std::flush;
      },
      // Receivers
      std::map<const std::string, Receiver>{}));
  m.AddState(std::make_unique<State>(
      // State Name
      "second",
      // Parent State
      "",
      // On Entry Action
      [&]() {
        secondOnEntry++;
        info() << "secondOnEntry: " << secondOnEntry << '\n' << std::flush;
        m.GotoState("first");
      },
      // On Exit Action
      [&]() {
        secondOnExit++;
        info() << "secondOnExit: " << secondOnExit << '\n' << std::flush;
      },
      // Receivers
      std::map<const std::string, Receiver>{}));
  m.Start("second");

  ASSERT_EQ("127.0.0.1:42002 test/Test", m.Uid());

  // Bind call
  { ASSERT_EQ(1, socket.bindArgs.size()); }

  // Check socket receive timeout
  {
    ASSERT_EQ(1, socket.setReceiveTimeoutArgs.size());
    const auto arg = socket.setReceiveTimeoutArgs.at(0);
    ASSERT_EQ(REPLY_TIMEOUT, arg.seconds);
  }

  // Check socket send
  {
    ASSERT_EQ(4, socket.sendArgs.size());
    // Send Started Message to Spawner
    {
      const auto arg0 = socket.sendArgs.at(0);
      ASSERT_EQ(spawner.ip, arg0.toIP);
      ASSERT_EQ(spawner.port, arg0.toPort);
      ASSERT_EQ(std::string("started test/Test"), std::string(arg0.buffer));
      ASSERT_EQ(18, arg0.length);
    }
    // Register Machine
    {
      const auto arg1 = socket.sendArgs.at(1);
      ASSERT_EQ(LOCALHOST, arg1.toIP);
      ASSERT_EQ(SERVER_PORT, arg1.toPort);
      std::string message(arg1.buffer);
      ASSERT_TRUE(message.starts_with("register test/Test "));
    }
    // Send Exited Message to Spawner
    {
      const auto arg2 = socket.sendArgs.at(2);
      ASSERT_EQ(spawner.ip, arg2.toIP);
      ASSERT_EQ(spawner.port, arg2.toPort);
      ASSERT_EQ(std::string("exited test/Test"), std::string(arg2.buffer));
      ASSERT_EQ(17, arg2.length);
    }
    // Unregister Machine
    {
      const auto arg3 = socket.sendArgs.at(3);
      ASSERT_EQ(LOCALHOST, arg3.toIP);
      ASSERT_EQ(SERVER_PORT, arg3.toPort);
      ASSERT_EQ(std::string("unregister"), std::string(arg3.buffer));
      ASSERT_EQ(11, arg3.length);
    }
  }
  // Initial State Transitions
  ASSERT_EQ(1, firstOnEntry);
  ASSERT_EQ(1, firstOnExit);
  ASSERT_EQ(1, secondOnEntry);
  ASSERT_EQ(1, secondOnExit);
}

TEST(MachineTest, Start_Heartbeat) {
  // Repeatedly
  // - sendPulseToSpawner
  // - checkHealthOfSpawned
  // - receiveMessage
}

TEST(MachineTest, Exit) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);
  // Override exit
  m.onExit = []() {};
  m.Exit();

  // Check socket send
  ASSERT_EQ(2, socket.sendArgs.size());
  // Send Exited Message to Spawner
  const auto arg0 = socket.sendArgs.at(0);
  ASSERT_EQ(spawner.ip, arg0.toIP);
  ASSERT_EQ(spawner.port, arg0.toPort);
  ASSERT_EQ(std::string("exited test/Test"), std::string(arg0.buffer));
  ASSERT_EQ(17, arg0.length);
  // Unregister Machine
  const auto arg1 = socket.sendArgs.at(1);
  ASSERT_EQ(LOCALHOST, arg1.toIP);
  ASSERT_EQ(SERVER_PORT, arg1.toPort);
  ASSERT_EQ(std::string("unregister"), std::string(arg1.buffer));
  ASSERT_EQ(11, arg1.length);
}

TEST(MachineTest, Error) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);
  socket.sendResults.push_back(result);
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);
  // Override exit
  m.onExit = []() {};
  m.Error("AHHHH");

  // Check socket send
  ASSERT_EQ(3, socket.sendArgs.size());
  // Send Errored Message to Spawner
  const auto arg0 = socket.sendArgs.at(0);
  ASSERT_EQ(spawner.ip, arg0.toIP);
  ASSERT_EQ(spawner.port, arg0.toPort);
  ASSERT_EQ(std::string("errored test/Test AHHHH"), std::string(arg0.buffer));
  ASSERT_EQ(24, arg0.length);
  // Send Exited Message to Spawner
  const auto arg1 = socket.sendArgs.at(1);
  ASSERT_EQ(spawner.ip, arg1.toIP);
  ASSERT_EQ(spawner.port, arg1.toPort);
  ASSERT_EQ(std::string("exited test/Test"), std::string(arg1.buffer));
  ASSERT_EQ(17, arg1.length);
  // Unregister Machine
  const auto arg2 = socket.sendArgs.at(2);
  ASSERT_EQ(LOCALHOST, arg2.toIP);
  ASSERT_EQ(SERVER_PORT, arg2.toPort);
  ASSERT_EQ(std::string("unregister"), std::string(arg2.buffer));
  ASSERT_EQ(11, arg2.length);
}

TEST(MachineTest, AddState) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);
  // TODO Test state is added to the vector of states
  // TODO Test first state becomes initial default
}

TEST(MachineTest, GotoState) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;
  Machine m(spawner, address, name, socket);

  int firstOnEntry = 0;
  int firstOnExit = 0;
  int secondOnEntry = 0;
  int secondOnExit = 0;

  m.AddState(std::make_unique<State>(
      // State Name
      "first",
      // Parent State
      "",
      // On Entry Action
      [&]() { firstOnEntry++; },
      // On Exit Action
      [&]() { firstOnExit++; },
      // Receivers
      std::map<const std::string, Receiver>{}));

  m.AddState(std::make_unique<State>(
      // State Name
      "second",
      // Parent State
      "",
      // On Entry Action
      [&]() { secondOnEntry++; },
      // On Exit Action
      [&]() { secondOnExit++; },
      // Receivers
      std::map<const std::string, Receiver>{}));

  m.GotoState("second");

  ASSERT_EQ(0, firstOnEntry);
  ASSERT_EQ(1, firstOnExit);
  ASSERT_EQ(1, secondOnEntry);
  ASSERT_EQ(0, secondOnExit);
}

TEST(MachineTest, Send) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);

  Address destination(":42003");
  m.Send(destination, TEST_MESSAGE);

  // Check socket send
  ASSERT_EQ(1, socket.sendArgs.size());
  const auto arg = socket.sendArgs.at(0);
  ASSERT_EQ(destination.ip, arg.toIP);
  ASSERT_EQ(destination.port, arg.toPort);
  ASSERT_EQ(std::string(TEST_MESSAGE), std::string(arg.buffer));
  ASSERT_EQ(10, arg.length);
}

TEST(MachineTest, SendSelf) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);
  m.SendSelf(TEST_MESSAGE);

  // Check socket send
  ASSERT_EQ(1, socket.sendArgs.size());
  const auto arg = socket.sendArgs.at(0);
  ASSERT_EQ(address.ip, arg.toIP);
  ASSERT_EQ(address.port, arg.toPort);
  ASSERT_EQ(std::string(TEST_MESSAGE), std::string(arg.buffer));
  ASSERT_EQ(10, arg.length);
}

TEST(MachineTest, SendSpawner) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);
  m.SendSpawner(TEST_MESSAGE);

  // Check socket send
  ASSERT_EQ(1, socket.sendArgs.size());
  const auto arg = socket.sendArgs.at(0);
  ASSERT_EQ(spawner.ip, arg.toIP);
  ASSERT_EQ(spawner.port, arg.toPort);
  ASSERT_EQ(std::string(TEST_MESSAGE), std::string(arg.buffer));
  ASSERT_EQ(10, arg.length);
}

TEST(MachineTest, Spawn_Local) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);
  m.Spawn("useless/Useless");

  // Check socket send
  ASSERT_EQ(1, socket.sendArgs.size());
  const auto arg = socket.sendArgs.at(0);
  ASSERT_EQ(LOCALHOST, arg.toIP);
  ASSERT_EQ(SERVER_PORT, arg.toPort);
  ASSERT_EQ(std::string("start useless/Useless :0"), std::string(arg.buffer));
  ASSERT_EQ(25, arg.length);
}

TEST(MachineTest, Spawn_Remote) {
  Address spawner(":42001");
  Address address(":42002");
  std::string name("test/Test");
  MockSocket socket;

  // Set mock send result
  SendResult result = 0;
  socket.sendResults.push_back(result);

  Machine m(spawner, address, name, socket);
  Address destination(TEST_IP, TEST_PORT);
  m.Spawn("useless/Useless", destination);

  // Check socket send
  ASSERT_EQ(1, socket.sendArgs.size());
  const auto arg = socket.sendArgs.at(0);
  ASSERT_EQ(TEST_IP, arg.toIP);
  ASSERT_EQ(SERVER_PORT, arg.toPort);
  ASSERT_EQ(std::string("start useless/Useless :42424"),
            std::string(arg.buffer));
  ASSERT_EQ(29, arg.length);
}
