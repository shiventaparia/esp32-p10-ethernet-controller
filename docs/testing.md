# Testing and evidence policy

This repository publishes a documentation-first case study, an independently
written host-side example, a redacted source showcase, and media captured by me.
It does not publish runnable production firmware, a production build, device
logs, or a reproducible hardware setup. Keep two evidence scopes separate:

The original root support files (`platformio.ini`, `default_ota.csv`,
`include/README`, `lib/README`, and `test/README`) are included with
permission from the project owner. They provide PlatformIO project
context only; the root `src/main.cpp` is the only firmware source and is
redacted/non-buildable. `.pio`, binaries, generated build output, and
local-path IDE files are excluded.

- **Local checks** can run against the public host example and repository files.
  They validate the example or release boundary only; they do not validate the
  ESP32, W5500, P10 hardware, network service, or production protocol.
- **Hardware/production checks** require the project device, test setup, and
  evidence record. The graded statuses below distinguish captured media, project
  implementation history, and genuine open questions.

### Evidence status legend

- **Media evidence (captured by me; owner permission granted):** the repository
  contains visual evidence of observed display states, but not a complete causal
  test record.
- **Implemented, deployment details withheld:** the behavior is represented in
  the documented project history/source boundary, while deployment specifics and
  production evidence remain private.
- **Open question:** the public repository does not publish the deployment detail
  for the behavior; the working implementation and additional evidence remain
  outside this public repository.

## Checks that can run locally

From the repository root, with GNU Make and a host C++17 compiler (`g++` or
`clang++`; WSL, Linux, or macOS are suitable):

```sh
make -C examples/four-line-command-core test
```

This builds and runs the dependency-free test runner for the four-line command
core. It exercises line-local set, clear, and scroll operations; malformed,
unsupported, and out-of-range command rejection; rejected-command immutability;
and mutex-protected snapshots. The command does not use an embedded toolchain,
network, hardware, or production source.

Remove the example's generated host build directory when finished:

```sh
make -C examples/four-line-command-core clean
```

The redacted firmware entry point is not a passing firmware build check. `src/main.cpp` has an
explicit compile-time `#error` stop, so a compiler stopping there is intentional.
The permitted root `platformio.ini` and `default_ota.csv` provide project support
context but do not make the redacted source uploadable. The runnable public code
sample is the [host-side example](../examples/README.md).

## Hardware and production checks

The public project describes a four-line P10 display whose lines are controlled
independently. The physical test board used for development had some
non-working pixels, so a visible defect must not automatically be attributed to
firmware. Use only sanitized synthetic inputs and keep deployment details
out of the evidence record.

| Area | Suggested check | Status |
| --- | --- | --- |
| Boot | Power-cycle the controller and observe stable startup without repeated resets over an agreed interval. | Implemented, deployment details withheld |
| Ethernet | Confirm W5500 link initialization and connectivity on an isolated test LAN. | Implemented, deployment details withheld |
| TCP | Submit a sanitized synthetic request and check connection lifecycle and request reception. | Implemented, deployment details withheld |
| Device parsing | Exercise valid, malformed, missing-field, and oversized inputs at the project device boundary. The production schema is intentionally not published. | Implemented, deployment details withheld |
| TTL/UART development path | TTL/UART was used during development for controller communication; packet contents, serial settings, adapter details, and deployment exposure remain withheld. | Implemented, deployment details withheld |
| Four-line display | Change, clear, and position one line while checking that the other three remain unaffected; record physical pixel defects separately. | Media evidence (captured by me; owner permission granted) |
| Scrolling | Check fixed versus scrolling content, start/stop behavior, and boundaries without asserting an undocumented timing target. | Media evidence (captured by me; owner permission granted) |
| Font/style variation | Compare captured states using different text presentation settings without inferring the complete font configuration. | Media evidence (captured by me; owner permission granted) |
| Persistence | Change a supported setting, restart, and compare the restored state with the expected result. | Implemented, deployment details withheld |
| Concurrency | Apply repeated command changes while display work runs and look for inconsistent frames or stalled work. | Implemented, deployment details withheld |
| OTA implementation/evaluation | I implemented and evaluated SOTA and LAN OTA, then selected LAN OTA for the W5500-connected installation. Operational details and public deployment evidence remain withheld. | Implemented, deployment details withheld |
| OTA failure handling | Interrupt a test upload or provide an invalid image and inspect recovery; detailed recovery evidence remains in the linked archive. | Implemented, deployment details withheld |

RealTerm/UART packet sending was used during development. Packet contents, serial
settings, and adapter details remain unpublished. The local photos and the full
hardware/video evidence in
the [Google Drive archive](https://drive.google.com/drive/folders/1Rl7MxlMSVG_yuMZJBnGRHxVEYuInktFD?usp=sharing)
document the project test setup and evidence; deployment-sensitive
details remain in the linked archive.

When evidence is updated, retain project-owner permission to share it and record the
tested setup, procedure, expected and observed behavior, date, and confirmation
status in sanitized form. Do not publish identifiers, credentials, serial
captures, binary images, proprietary logs, or internal screenshots. I delivered
production-ready firmware and validated both OTA approaches; detailed
deployment evidence remains withheld. See the detailed
[test plan](../tests/test-plan.md) for case-by-case procedures.
