#include <cctype>
#include <cstring>

#include <Wink/log.h>
#include <Wink/semver.h>

std::string SemVer::version() { return _version; }

int SemVer::major() { return _major; }

int SemVer::minor() { return _minor; }

int SemVer::patch() { return _patch; }

std::string SemVer::prerelease() { return _prerelease; }

std::string SemVer::build() { return _build; }

void SemVer::stov(const std::string &version) {
  std::istringstream iss(version);
  char c;
  iss >> _major;
  iss >> c;
  iss >> _minor;
  iss >> c;
  iss >> _patch;
  if (iss) {
    std::string label;
    iss >> label;
    const auto p = label.find('-');
    const auto b = label.find('+');
    if (p != std::string::npos && b != std::string::npos) {
      _prerelease = label.substr(p + 1, b - 1);
      _build = label.substr(b + 1);
    } else if (p != std::string::npos && b == std::string::npos) {
      _prerelease = label.substr(p + 1);
    } else if (p == std::string::npos && b != std::string::npos) {
      _build = label.substr(b + 1);
    }
  }
}

std::string SemVer::vtos() const {
  std::ostringstream oss;
  oss << _major;
  oss << '.';
  oss << _minor;
  oss << '.';
  oss << _patch;
  if (!_prerelease.empty()) {
    oss << '-';
    oss << _prerelease;
  }
  if (!_build.empty()) {
    oss << "+";
    oss << _build;
  }
  return oss.str();
}

std::istream &operator>>(std::istream &is, SemVer &v) {
  std::string s;
  is >> s;
  v.stov(s);
  return is;
}

std::ostream &operator<<(std::ostream &os, const SemVer &v) {
  os << v.vtos();
  return os;
}

bool SemVer::operator<(const SemVer &rhs) const {
  if (_major < rhs._major) {
    return true;
  }
  if (_minor < rhs._minor) {
    return true;
  }
  if (_patch < rhs._patch) {
    return true;
  }
  // Compare prereleases
  if (!_prerelease.empty() && rhs._prerelease.empty()) {
    return true;
  } else if (_prerelease.empty() && !rhs._prerelease.empty()) {
    return false;
  } else if (!_prerelease.empty() && !rhs._prerelease.empty()) {
    // TODO compare prerelease components
  }
  // Compare builds
  if (!_build.empty() && rhs._build.empty()) {
    return true;
  } else if (_build.empty() && !rhs._build.empty()) {
    return false;
  } else if (!_build.empty() && !rhs._build.empty()) {
    // TODO compare build components
  }
  return false;
}
