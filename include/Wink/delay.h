#ifndef DELAY_H
#define DELAY_H

#include <ctime>
#include <functional>
#include <iostream>
#include <unistd.h>

#include <Wink/log.h>

/**
 * Calls the given lambda in a new process after the given time delay has
 * elapsed.
 */
void CallAfter(std::function<void()> f, std::time_t d);

#endif
