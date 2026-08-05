# Verification Record

Verification was performed on a Linux x86-64 environment using:


`gcc (Debian 14.2.0-19) 14.2.0`

The source files used for verification were copied from the portfolio directories without modification.

## Build checks

| Project | Command | Result |
|---|---|---|
| HTTP socket client | `make` | Compiled successfully |
| TCP header/checksum service | `make` | Client and server compiled successfully |
| Tahoe congestion-control simulation | `make` | Client and server compiled successfully |

## Runtime smoke checks

### TCP header/checksum service

The unchanged server and client were run locally for both main branches:

- `data`: the client received the input fields and printed a 20-byte TCP header;
- invalid command followed by `time`: the server re-prompted and returned a formatted local timestamp.

This was a functional smoke check of the submitted implementation. It was not an independent protocol-conformance certification.

### Tahoe congestion-control simulation

The unchanged client and server completed the configured ten rounds. The generated congestion-window trace was:

`1, 2, 4, 8, 9, 1, 2, 4, 1, 2`

The runtime output showed the intended transitions through slow start, congestion avoidance, repeated ACK handling, and retransmission.

### HTTP socket client

The program compiled successfully. It was not included in the runtime smoke test because it depends on a specific external host and live network access.

## Integrity check

After packaging, every preserved artifact was compared with its extracted original using both SHA-256 hashes and byte-wise comparison. The result is recorded in `ORIGINAL_FILE_MANIFEST.tsv`.
