#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Wink/log.h>

std::ostream &error() {
  std::cerr << "Error: ";
  return std::cerr;
}

std::ostream &info() { return std::cout; }

/*
Configure info and error logging output to directory.
*/
int logToFile(const std::string &directory, const std::string &name) {
  struct stat st = {0};

  if (const auto d = directory.c_str(); stat(d, &st) == -1) {
    if (const auto result = mkdir(d, 0777); result < 0) {
      error() << "Failed to make log directory: " << strerror(errno) << '\n'
              << std::flush;
      return -1;
    }
  }

  std::time_t t = std::time(nullptr);
  std::tm tm = *std::gmtime(&t);

  std::ostringstream filename;
  filename << std::put_time(&tm, "%Y%m%d%H%M%S");
  filename << name;
  filename << ".log";
  std::filesystem::path filepath(directory);
  filepath /= filename.str();

  int fd = open(filepath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    error() << "Failed to open file: " << filepath << ": " << strerror(errno)
            << '\n'
            << std::flush;
    return -1;
  }

  if (const auto result = dup2(fd, 1); result < 0) {
    error() << "Failed to redirect std::cout output to log file\n"
            << std::flush;
    close(fd);
    return -1;
  }

  if (const auto result = dup2(fd, 2); result < 0) {
    error() << "Failed to redirect std::cerr output to log file\n"
            << std::flush;
    close(fd);
    return -1;
  }

  return close(fd);
}
