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

Demonstrates different tree shapes of Hierarchical State Machines;

- Empty - a tree with no nodes
- Leaf - a tree with a single leaf node.
- Simple - a tree with a parent and two child leaf nodes.
- Bigger - a tree with three levels of nodes.
- Forrest - a set of multiple trees ranging in size and shape.

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

Demonstrates various time-related use cases;
- After - send a message after a given delay
- At - send a message at a specific time

### Usage

```
start time/After
start time/At
```

## Useless

A Useless State Machine that exits as soon as it is started.

### Usage

```
start useless/Useless
```
