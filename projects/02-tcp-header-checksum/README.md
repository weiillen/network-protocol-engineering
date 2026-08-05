# TCP Header and Checksum Service

A local C client/server program that combines socket programming with manual TCP ACK-header construction.

## What it demonstrates

- creation, binding, listening, acceptance, and connection of TCP sockets;
- interactive `data`, `time`, and invalid-command handling;
- use of `SO_REUSEADDR` on the server;
- conversion of multi-byte fields to network byte order;
- construction of an IPv4 TCP pseudo-header;
- creation of a 20-byte TCP header without options;
- computation of a TCP checksum using one's-complement addition.

## Files

- `server.c` — accepts local connections and serves data or the current time;
- `client.c` — sends a command and constructs the TCP header for the data path;
- `header.c`, `header.h` — shared structures, parsing, and display helpers;
- `sample_input.txt` — preserved input fields;
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
./server sample_input.txt
```

Terminal 2:

```bash
./client
```

At the prompt, enter:

- `data` to receive the field structure and print the generated TCP header;
- `time` to receive the server's local time;
- any other string to exercise the invalid-command path.

Stop the server with `Ctrl+C`.

## Header construction flow

1. Read source/destination IPv4 addresses and TCP field values.
2. Build the 12-byte pseudo-header.
3. Serialize the fixed 20-byte ACK header with a zero checksum field.
4. Sum 16-bit words from the pseudo-header and TCP header.
5. Fold carry bits and store the one's complement in the checksum field.

## Known limitations

- The service is fixed to loopback address `127.0.0.1` and port `45525`.
- The server handles one accepted client at a time.
- A complete in-memory `Segment` structure is sent directly through the TCP stream, which assumes compatible structure layout on both processes.
- Individual `send()` and `recv()` calls are treated as if they transfer a complete object.
- Some byte-array fields are populated through casted integer pointers, which can depend on platform alignment behavior.
- The checksum demonstration assumes a pure TCP ACK with no payload and a 20-byte header.

These are documented characteristics of the original implementation, not post-hoc corrections.
