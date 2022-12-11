#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef unsigned int uint;
typedef unsigned short ushort;

constexpr ushort MAX_PAYLOAD = 65507;

constexpr ushort NO_TIMEOUT = 0;         // Unlimited
constexpr ushort REPLY_TIMEOUT = 1;      // 1 second
constexpr ushort HEARTBEAT_TIMEOUT = 10; // 10 seconds
constexpr ushort RECEIVE_TIMEOUT = 600;  // 10 minutes

constexpr ushort PULSE_INTERVAL = 2; // 2 seconds

constexpr ushort SERVER_PORT = 42000;

constexpr char LOCALHOST[] = "127.0.0.1";

#endif
