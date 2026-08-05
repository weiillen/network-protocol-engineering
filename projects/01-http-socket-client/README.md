# Raw HTTP Socket Client

A compact C client that performs an HTTP request without using a higher-level HTTP library.

## What it demonstrates

- creation of an IPv4 TCP socket;
- hostname resolution with `gethostbyname`;
- connection to a web server on port 80;
- manual construction and transmission of an HTTP/1.1 GET request;
- repeated `recv()` calls until the server closes the stream;
- extraction of text enclosed by `<h2>` and `</h2>` tags.

## Files

- `client.c` — original implementation;
- `makefile` — original build instructions.

Both files are preserved unchanged.

## Build and run

```bash
make
./client
```

The host and HTTP request are hard-coded in `client.c`. Runtime success therefore depends on DNS, Internet access, the remote server remaining available, and the server continuing to support unencrypted HTTP on port 80.

## Design notes

The program demonstrates the application-layer request/response flow directly on top of a TCP byte stream. The HTML parsing is intentionally minimal: it searches individual receive buffers for literal lowercase `<h2>` tags.

## Known limitations

- The destination host and request path are fixed in the source.
- HTTPS/TLS is not supported.
- The request does not explicitly send `Connection: close`.
- A tag split across two separate `recv()` calls may not be detected.
- `gethostbyname` is an older IPv4-only resolver interface.

These limitations are retained because the portfolio version does not modify the original code.
