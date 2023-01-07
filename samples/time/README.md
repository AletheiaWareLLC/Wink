# Time

Demonstrates various time-related use cases.

## After

Send a message after a given delay.

### Usage

```
start time/After
```

## At

Send a message at a specific time.

### Usage

```
start time/At
```

## StopWatch

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

### Usage

```
start time/StopWatch
send <stopwatch> start
send <stopwatch> stop

```
