#ifndef SEMVER_H
#define SEMVER_H

#include <sstream>
#include <string>

class SemVer {
public:
  SemVer() {}
  SemVer(std::string version) : _version(version) { stov(version); }
  SemVer(int major, int minor, int patch, std::string prerelease,
         std::string build)
      : _major(major), _minor(minor), _patch(patch), _prerelease(prerelease),
        _build(build) {
    _version = vtos();
  }
  ~SemVer() {}
  std::string version();
  int major();
  int minor();
  int patch();
  std::string prerelease();
  std::string build();
  void stov(const std::string &version);
  std::string vtos() const;
  bool operator<(const SemVer &rhs) const;

private:
  std::string _version = "";
  int _major = 0;
  int _minor = 0;
  int _patch = 0;
  std::string _prerelease = "";
  std::string _build = "";
};

std::istream &operator>>(std::istream &is, SemVer &v);
std::ostream &operator<<(std::ostream &os, const SemVer &v);

#endif
