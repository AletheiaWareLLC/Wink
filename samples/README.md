# Samples

This directory contains samples of defining State Machines in Wink.

## Blinky

A State Machine for controlling an LED with a Raspberry Pi.

## Echo

A State Machine that replies to every message with an identical message.

### Usage

```
start echo/Echo
send <echo> test
```

## Family

Demonstrates how a parent State Machine can spawn and monitor a child State Machine.

### Usage

```
start parent/Parent
```

## FizzBuzz

A State Machine that accepts a number and prints 'fizz' if the number is divisible by 3, 'buzz' if the number is divisible by 5, and 'fizzbuzz' if the number is divisible by 3 and 5.

### Usage

```
start fizzbuzz/FizzBuzz
send <fizzbuzz> 1
send <fizzbuzz> 2
send <fizzbuzz> 3
```

## Forward

A State Machine that sends all received messages to another State Machine.

### Usage

```
start forward/Forward <destination>
```

## Hierarchy

Demonstrates a Hierarchical State Machine.

## Network

Demonstrates how State Machines running in isolated Docker containers can communicate across a Docker network.

```
docker compose up
```

## Switch

A simple on/off toggle switch.

### Usage

```
start switch/Switch
send <switch> on
send <switch> off
```

## Time

Demonstrates various time-related use cases.

### After

Send a message after a given delay.

#### Usage

```
start time/After
```

### At

Send a message at a specific time.

#### Usage

```
start time/At
```

### StopWatch

Implements a StopWatch for calculating how much time has elapsed between the watch being started and stopped.

This state machine has two states; `idle` and `timing`.

When the machine is in state `idle` and receives;

- idle - the machine transitions back to idle.
- start - the machine records the current time and transitions to timing.
- stop - the machine transitions back to idle.
- exit - the machine exits.

When the machine is in state `timing` and receives;

- stop - the machine records the current time, send the elapsed time to the sender, and transitions back to `idle`.

The state `idle` is the parent of timing, so when timing receives an `idle`, `start`, or `exit` message, it is handled by the parent state.

#### Usage

```
start time/StopWatch
send <stopwatch> start
send <stopwatch> stop

```

## Useless

A Useless State Machine that exits as soon as it is started.

### Usage

```
start useless/Useless
```
