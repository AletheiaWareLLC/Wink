#ifndef LOG_H
#define LOG_H

#include <chrono>
#include <iostream>

std::ostream &error();
std::ostream &info();

int logToFile(const std::string &directory, const std::string &name);

#endif
