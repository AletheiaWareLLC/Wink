#ifndef DELAY_H
#define DELAY_H

#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <unistd.h>

#include <Wink/log.h>

/**
 * After forks a new process which waits the given duration in seconds before
 * calling the given function.
 */
void After(std::time_t d, std::function<void()> f);

#endif
