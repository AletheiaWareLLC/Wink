#ifndef DELAY_H
#define DELAY_H

#include <functional>
#include <iostream>
#include <unistd.h>

#include <Wink/log.h>

void CallAfter(std::function<void()> f, std::time_t d);

#endif
