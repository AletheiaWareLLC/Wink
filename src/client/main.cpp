#include <Wink/client.h>

void usage(std::string name) {
  info() << name << '\n';
  info() << "\tstart [options] <binary> <host>\n";
  info() << "\tstop [options] <machine>\n";
  info() << "\tsend [options] <machine> <message>\n";
  info() << "\tlist [options] <host>\n";
  info() << "\thelp\n";
}

void usage() { usage("Wink"); }

void help(std::string name, std::string command) {
  if (command == "start") {
    info() << "Start a new machine.\n\n";
    info() << "Options;";
    info() << "\n\t-a\n\t\tThe address to bind to (default " << LOCALHOST
           << ")\n";
    info()
        << "\n\t-f\n\t\tFollow the lifcycle of the machine (default false)\n";
    info() << "Parameters;";
    info() << "\n\tbinary\n\t\tThe machine binary to start\n";
    info() << "\n\thost\n\t\tThe host to start the machine on\n";
    info() << "Examples;";
    info() << "\n\tstart machine.bin\n\t\tStart a new machine on localhost "
              "on any available port\n";
    info() << "\n\tstart machine.bin :64646\n\t\tStart a new machine on "
              "localhost port 64646\n";
    info() << "\n\tstart machine.bin 123.45.67.89\n\t\tStart a new machine "
              "on ip 123.45.67.89 any available port\n";
    info() << "\n\tstart machine.bin 123.45.67.89:64646\n\t\tStart a new "
              "machine on ip 123.45.67.89 port 64646\n";
  } else if (command == "stop") {
    info() << "Stop an existing machine.\n\n";
    info() << "Options;";
    // info() << "\n\t--test\n\t\tTest (default 5)\n";
    info() << "Parameters;";
    info() << "\n\tmachine\n\t\tThe machine to stop\n";
    info() << "Examples;";
    info() << "\n\tstop 123.45.67.89:64646\n\t\tStop an existing machine on ip "
              "123.45.67.89 port 64646\n";
  } else if (command == "send") {
    info() << "Sends a message to a machine\n\n";
    info() << "Options;";
    // info() << "\n\t--test\n\t\tTest (default 5)\n";
    info() << "Parameters;";
    info() << "\n\tmachine\n\t\tThe machine to send to\n";
    info() << "\n\tmessage\n\t\tThe message to send\n";
    info() << "Examples;";
    info() << "\n\tsend :64646 add(2,8)\n\t\tSend a message to machine on "
              "localhost port 64646\n";
    info() << "\n\tsend 123.45.67.89:64646 add(2,8)\n\t\tSend a message to "
              "machine on ip 123.45.67.89 port 64646\n";
  } else if (command == "list") {
    info() << "List machines running on a host\n\n";
    info() << "Options;";
    // info() << "\n\t--test\n\t\tTest (default 5)\n";
    info() << "Parameters;";
    info() << "\n\thost\n\t\tThe host to list the machines from\n";
    info() << "Examples;";
    info()
        << "\n\tlist\n\t\tLists the machines running on localhost port 42000\n";
    info() << "\n\tlist :64646\n\t\tLists the machines running on localhost "
              "port 64646\n";
    info() << "\n\tlist 123.45.67.89:64646\n\t\tLists the machines running on "
              "ip 123.45.67.89 port 64646\n";
  } else {
    usage();
  }
}

int main(int argc, char **argv) {
  if (argc <= 0) {
    usage();
    return 0;
  }
  std::string name(argv[0]);
  if (argc == 1) {
    usage(name);
    return 0;
  }

  std::string command(argv[1]);

  std::map<std::string, std::string> options;
  std::vector<std::string> parameters;
  for (int i = 2; i < argc; ++i) {
    if (argv[i][0] == '-' && i + 1 < argc) {
      options.insert(std::pair<std::string, std::string>(
          std::string(argv[i]), std::string(argv[i + 1])));
      i++;
    } else {
      parameters.push_back(std::string(argv[i]));
    }
  }

  if (command == "start") {
    Address address(LOCALHOST, 0);
    bool follow = false;

    // Parse Options
    for (const auto &[k, v] : options) {
      if (k == "-a") {
        std::stringstream ss(v);
        ss >> address;
      } else if (k == "-f") {
        std::stringstream ss(v);
        ss >> follow;
      } else {
        error() << "Option " << k << ":" << v << " not supported\n"
                << std::flush;
      }
    }

    std::string binary;
    Address destination(LOCALHOST, 0);
    std::vector<std::string> args;
    switch (parameters.size()) {
    case 0:
      error() << "Missing <binary> parameter\n" << std::flush;
      return -1;
    case 1:
      binary = parameters.at(0);
      break;
    default: {
      binary = parameters.at(0);
      std::istringstream ss(parameters.at(1));
      ss >> destination;
      for (uint i = 2; i < parameters.size(); i++) {
        args.push_back(parameters.at(i));
      }
      break;
    }
    }

    UDPSocket socket;
    if (const auto result =
            StartMachine(socket, address, binary, destination, args, follow);
        result < 0) {
      return result;
    }
  } else if (command == "stop") {
    // TODO Parse Options
    for (const auto &[k, v] : options) {
      error() << "Option " << k << ":" << v << " not supported\n" << std::flush;
    }

    Address address(LOCALHOST, 0);
    switch (parameters.size()) {
    case 0:
      error() << "Missing <machine> parameter\n" << std::flush;
      return -1;
    case 1: {
      std::istringstream ss(parameters.at(0));
      ss >> address;
      break;
    }
    default:
      error() << "Too many parameters\n" << std::flush;
      return -1;
    }

    UDPSocket socket;
    return StopMachine(socket, address);
  } else if (command == "send") {
    // TODO Parse Options
    for (const auto &[k, v] : options) {
      error() << "Option " << k << ":" << v << " not supported\n" << std::flush;
    }

    Address address(LOCALHOST, 0);
    std::string message;
    switch (parameters.size()) {
    case 0:
      error() << "Missing <machine> parameter\n" << std::flush;
      return -1;
    case 1:
      error() << "Missing <message> parameter\n" << std::flush;
      return -1;
    case 2: {
      const auto s = parameters.at(0);
      std::istringstream ss(s);
      ss >> address;
      message = parameters.at(1);
      break;
    }
    default:
      error() << "Too many parameters\n" << std::flush;
      return -1;
    }

    UDPSocket socket;
    return SendMessage(socket, address, message);
  } else if (command == "list") {
    // TODO Parse Options
    for (const auto &[k, v] : options) {
      error() << "Option " << k << ":" << v << " not supported\n" << std::flush;
    }

    Address address(LOCALHOST, SERVER_PORT);
    switch (parameters.size()) {
    case 0:
      break;
    case 1: {
      const auto s = parameters.at(0);
      std::istringstream ss(s);
      ss >> address;
      break;
    }
    default:
      error() << "Too many parameters\n" << std::flush;
      return -1;
    }

    UDPSocket socket;
    return ListMachines(socket, address);
  } else if (command == "help") {
    if (argc < 3) {
      usage();
    } else {
      help(name, std::string(argv[2]));
    }
  } else {
    usage(name);
  }
  return 0;
}
