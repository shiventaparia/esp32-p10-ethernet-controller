# Redacted firmware source showcase

This directory contains the release-reviewed documentation for an intentionally
non-buildable source showcase based on the authorized ESP32 P10 controller
project. It is a source review artifact, not a firmware release and not a second
runnable example.

The single redacted firmware entry point is now [`../src/main.cpp`](../src/main.cpp)
so the repository has one root PlatformIO source path. This directory retains
the showcase documentation and context-only files; it does not contain a second
copy of the firmware source.

## Build boundary

`src/main.cpp` contains an explicit compile-time `#error` near the top of the file.
That stop is intentional: compiling the file must fail before it can be
uploaded. Do not treat that failure as a broken build, and do not run PlatformIO
here expecting uploadable firmware. A quick local guard check is:

From the repository root:

```sh
grep -n '^#error ' src/main.cpp
```

The runnable public code sample is the independently written host-side
[four-line command core](../examples/four-line-command-core/), which can be run
with the commands in [`examples/README.md`](../examples/README.md).

## Included

- ESP32/DMD display state and illustrative row-slot organization. The public
  runnable model of the project behavior is the four-line independently
  controlled host example, not this generic source sketch.
- Preferences/NVS persistence for display state.
- Font selection, row rendering, scrolling state, and timer-driven scanning.
- FreeRTOS task structure and mutex-protected display access.
- A small display-command boundary using synthetic example data; it is not the
  production command schema.
- The permitted root PlatformIO support files are documented separately;
  they provide context but do not make this redacted source buildable.

## Intentionally omitted

The following sections are represented by omission markers rather than copied
from the production firmware:

- Production TCP server implementation and command schema.
- Network addresses, MAC addresses, ports, deployment defaults, and diagnostics.
- Network configuration persistence and operational management commands.
- LAN OTA HTTP request handling, upload routes, and flash-update implementation.
- Customer-specific display content, operational upload commands, and local logs.

The omissions are intentional. The file is not a drop-in firmware build, and its
explicit compile-time stop prevents accidental upload. The independent
host-side example in `../examples/four-line-command-core/` is the runnable
public code sample.

## Why sections were omitted

| Omitted section | Reason for omission |
| --- | --- |
| TCP server and production command schema | Prevents publishing the deployed transport contract, framing behavior, and operational request vocabulary. |
| Network defaults and configuration commands | Prevents publishing deployment identifiers, service settings, and device-management details. |
| LAN OTA HTTP handler and upload route | Keeps firmware-update operations, routes, and deployment workflow out of the source showcase. |
| Production diagnostics and sample content | Prevents customer-facing content, runtime addresses, and operational logs from becoming part of the public source. |

These removals do not imply that the omitted features were absent from the
project. They identify the portions intentionally withheld from this public
release.

## Source relationship

This showcase was prepared from the project for portfolio review with permission
from the project owner. It
is not a complete copy of the production firmware. The original project folder
and its `.pio/` build outputs remain outside this repository; no production
source, firmware binaries, generated build output, or local-path IDE files are
included here. The root `src/main.cpp` is the only firmware source in the
repository and remains redacted/non-buildable. The original root support files
are included separately with permission from the project owner; they
provide context only and do not change this source boundary.

The permitted root `platformio.ini` and `default_ota.csv` are included as
support context. They do not make the
redacted `src/main.cpp` buildable, and they do not authorize adding any other
production source or generated output.

## Third-party dependencies

The include names indicate the technology areas used by the project. Upstream
libraries are not copied into this repository; consult their original licenses
and documentation before using them.
