#include <Wink/delay.h>

void CallAfter(std::function<void()> f, std::time_t d) {
  // Fork child process
  pid_t pid;
  switch (pid = fork()) {
  case -1:
    error() << "Failed to fork process: " << std::strerror(errno) << '\n'
            << std::flush;
  case 0: {
    // Child sleeps for the delay, then calls function
    sleep(d);
    f();
    _exit(0);
  }
  default:
    // Parent does nothing
    info() << "Forked: " << pid << '\n' << std::flush;
  }
}
