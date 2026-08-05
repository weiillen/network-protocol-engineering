# Network Protocol Engineering in C

A preserved portfolio collection of three C networking implementations that move from application-layer communication to transport-layer packet construction and congestion-control behavior.

The source files in this repository are the original submitted implementations. They have **not** been refactored, corrected, reformatted, or modernized for the portfolio. The surrounding documentation explains what the programs demonstrate, how to build them, and where their limitations are.

## Project overview

| Project | Focus | Main concepts |
|---|---|---|
| [HTTP socket client](projects/01-http-socket-client/) | Send an HTTP/1.1 request over a raw TCP socket and extract `<h2>` text from the response | DNS resolution, TCP client lifecycle, HTTP request structure, stream reception |
| [TCP header and checksum service](projects/02-tcp-header-checksum/) | Exchange commands through a local client/server pair and construct a 20-byte TCP ACK header from input fields | `bind`/`listen`/`accept`, network byte order, pseudo-header construction, one's-complement checksum |
| [Tahoe congestion-control simulation](projects/03-tahoe-congestion-control/) | Simulate sequence numbers, acknowledgements, packet loss, duplicate ACKs, retransmission, and congestion-window updates | Slow start, congestion avoidance, `cwnd`, `ssthresh`, fast retransmit |

## Technical progression

### 1. Application-layer traffic over TCP

The first program creates a TCP socket, resolves a fixed hostname, connects to port 80, sends an HTTP GET request, receives the byte-stream response, and searches the returned HTML for heading tags.

### 2. TCP metadata and checksum construction

The second program expands the socket workflow into a local client/server service. The server supports `data` and `time` commands. For the data path, it reads Layer 3 and Layer 4 values from a text file and sends them to the client. The client then constructs:

- a 12-byte IPv4 pseudo-header;
- a 20-byte TCP ACK header without options;
- the TCP checksum using 16-bit one's-complement addition.

### 3. Transport behavior under simulated loss

The third program models a simplified TCP Tahoe-style sender and receiver. It records congestion-window values while transitioning between slow start and congestion avoidance, and resets the congestion window after detecting repeated acknowledgements.

The preserved output sequence is:

```text
1, 2, 4, 8, 9, 1, 2, 4, 1, 2
```

This sequence is an output of this specific educational simulation, not a general TCP performance benchmark.

## Repository structure

```text
network-protocol-engineering-preserved/
├── README.md
├── PRESERVATION.md
├── ORIGINAL_FILE_MANIFEST.tsv
├── docs/
│   └── VERIFICATION.md
└── projects/
    ├── 01-http-socket-client/
    ├── 02-tcp-header-checksum/
    └── 03-tahoe-congestion-control/
```

Each project directory is independently buildable using its original `makefile`.

## Build environment

The implementations use POSIX socket APIs and are intended for macOS or Linux-like environments with GCC or Clang and `make` installed.

```bash
cd projects/01-http-socket-client
make
```

Repeat from the other project directories as needed. Detailed run instructions are provided in each project README.

## Verification

During portfolio packaging, all three projects compiled successfully from the preserved files. The two local client/server projects also completed smoke tests without source modification. See [docs/VERIFICATION.md](docs/VERIFICATION.md).

## Scope and limitations

These are compact educational implementations rather than production network stacks. Important limitations include:

- fixed addresses, ports, and protocol assumptions in several programs;
- no TLS support in the HTTP client;
- direct transmission of in-memory C structures in the local demonstrations;
- simplified packet-loss and acknowledgement behavior in the congestion-control model;
- single-process, single-client server structure rather than concurrent request handling.

The limitations are documented rather than silently corrected so the repository remains an accurate record of the original work.

## Provenance

The programs originated in an introductory computer-networks course and are curated here as a technical portfolio. Assignment sheets, reports, compiled binaries, editor settings, packet captures, and starter archives were intentionally excluded from the public-facing repository. See [PRESERVATION.md](PRESERVATION.md) for the exact preservation policy.
