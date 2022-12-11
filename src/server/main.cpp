#include <Wink/server/server.h>

void usage(std::string name) {
  info() << name << '\n';
  info() << "\tserve [options] <directory>\n";
  info() << "\thelp\n";
}

void usage() { usage("WinkServer"); }

void help(std::string name, std::string command) {
  if (command == "serve") {
    info() << "Starts the Wink Server to serve the given directory.\n\n";
    info() << "Options;";
    info() << "\n\t-a\n\t\tThe address to bind to (default " << LOCALHOST << ':'
           << SERVER_PORT << ")\n";
    info() << "\n\t-l\n\t\tThe directory to log to (default disabled)\n";
    info() << "Parameters;";
    info() << "\n\tdirectory\n\t\tThe directory containing machine files\n";
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

  if (command == "serve") {
    Address address(LOCALHOST, SERVER_PORT);
    UDPSocket socket;
    std::string log;

    if (parameters.size() == 0) {
      error() << "Missing <directory> parameter\n" << std::flush;
      return -1;
    }

    // Parse Options
    for (const auto &[k, v] : options) {
      if (k == "-a") {
        std::stringstream ss(v);
        ss >> address;
      } else if (k == "-l") {
        log = v;
      } else {
        error() << "Option " << k << ":" << v << " not supported\n"
                << std::flush;
      }
    }

    Server server(address, socket, log);

    const auto directory = parameters.at(0);
    return server.Serve(directory);
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
