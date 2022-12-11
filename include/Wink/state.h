#ifndef STATE_H
#define STATE_H

#include <functional>
#include <map>

typedef std::function<void(const Address &, std::istream &)> Receiver;

class State {
public:
  State(const std::string name, const std::string parent,
        const std::function<void()> onEnter, const std::function<void()> onExit,
        const std::map<const std::string, Receiver> receivers)
      : name(name), parent(parent), onEnter(onEnter), onExit(onExit),
        receivers(receivers) {}
  State(const State &m) = delete;
  State(State &&m) = delete;
  ~State() {}

  const std::string name;
  const std::string parent;
  const std::function<void()> onEnter;
  const std::function<void()> onExit;
  const std::map<const std::string, Receiver> receivers;
};

#endif