# Tahoe Congestion-Control Simulation

A local C client/server simulation of selected TCP Tahoe congestion-control behaviors.

## What it demonstrates

- sequence-numbered segment transmission;
- cumulative acknowledgement tracking;
- simulated packet loss;
- repeated ACK detection;
- fast retransmission of the missing sequence number;
- slow-start and congestion-avoidance updates;
- recording the congestion window to `cwnd.txt`.

## Files

- `server.c` — sender state, window updates, forced loss, ACK handling, and retransmission;
- `client.c` — receiver-side sequence tracking and ACK generation;
- `header.c`, `header.h` — shared segment structure and helper functions;
- `cwnd.txt` — preserved output from the original run;
- `makefile` — original build instructions.

All listed files are preserved unchanged.

## Build

```bash
make
```

## Run

Open two terminals in this directory.

Terminal 1:

```bash
./server
```

Terminal 2:

```bash
./client
```

The server performs ten rounds and writes the current congestion-window size at the beginning of each round.

## Preserved congestion-window trace

| Round | `cwnd` |
|---:|---:|
| 1 | 1 |
| 2 | 2 |
| 3 | 4 |
| 4 | 8 |
| 5 | 9 |
| 6 | 1 |
| 7 | 2 |
| 8 | 4 |
| 9 | 1 |
| 10 | 2 |

The early `1 → 2 → 4 → 8` progression represents exponential slow-start growth. The transition to `9` represents additive growth after reaching the initial threshold. The resets to `1` follow repeated-ACK loss handling in this particular simulation.

## Model boundaries

This is a teaching-oriented Tahoe-style emulator, not an implementation inside an operating-system TCP stack. It exchanges custom C structures over an already reliable TCP connection and uses a Boolean field to represent simulated loss.

## Known limitations

- The client and server are fixed to port `45525` and loopback communication.
- Sequence numbers represent simulated units rather than real TCP byte offsets.
- Loss is forced by the server's state logic rather than occurring in an actual network.
- The model processes ACKs in batches after sending a full congestion window.
- Complete structures are transmitted through single `send()`/`recv()` calls.
- The behavior is a simplified subset of Tahoe and should not be used as a protocol-conformance reference.

The portfolio documentation describes these constraints without modifying the implementation.
