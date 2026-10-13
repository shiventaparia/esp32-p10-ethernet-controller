# Runnable host-side example

[`four-line-command-core/`](four-line-command-core/) is a small,
independently written C++17 example. It provides a four-line display state in
which each line can be addressed independently, a deliberately limited
JSON-like command parser, command dispatch, and mutex-protected snapshots. Its
purpose is to make the case-study behavior and its validation boundaries
runnable without hardware.

## Run the local check

From the repository root, use GNU Make and a host C++17 compiler (`g++` or
`clang++`):

```sh
make -C examples/four-line-command-core test
```

The equivalent commands from this directory are:

```sh
cd four-line-command-core
make test
make clean
```

The test runner checks independent set, clear, and scroll behavior for all four
lines; malformed, unsupported, and out-of-range command rejection; state
remaining unchanged after rejection; and concurrent, mutex-protected snapshots.
It exits non-zero if a check fails. The `build/` directory contains generated
host objects and the test binary and can be removed with `make clean`; it is
local generated output, not a repository deliverable.

GNU Make plus a host C++17 toolchain is required; WSL, Linux, or macOS are
appropriate environments for the supplied Makefile. No network, hardware,
PlatformIO, or embedded toolchain is needed.

This example remains independent and runnable for illustration. It is not
production firmware, does not implement the production device protocol, and a
passing local run is not evidence of production behavior or hardware output.
The redacted showcase in [`../showcase/`](../showcase/) is a separate,
intentionally non-buildable source review artifact; the root `src/main.cpp` is
the repository's only firmware source.
