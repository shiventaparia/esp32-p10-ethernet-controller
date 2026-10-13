# Proposed validation plan

The public repository has one runnable local check: the independently written
host-side example. It models four independently controlled display lines, but
it is not embedded firmware and does not implement the production protocol.
The original root support files (`platformio.ini`, `default_ota.csv`,
`include/README`, `lib/README`, and `test/README`) are included with permission
from the project owner; they provide context and do not make the sole
root firmware source, `src/main.cpp`, buildable. That source is redacted and
intentionally non-buildable. `.pio`, binaries, generated build output, and
local-path IDE files are excluded.
The hardware/production cases use three graded statuses: **Media evidence
(captured by me; owner permission granted)**, **Implemented, deployment details withheld**, and **Open
question**. I delivered production-ready firmware and validated both OTA
approaches; deployment-sensitive details remain outside this repository.

Use the project test setup and sanitized synthetic inputs. For each case, record
preconditions, steps, expected and observed behavior, date, and documented
outcome in a sanitized form before changing a status. Avoid publishing serial
captures, network settings, packet contents, binary images, or internal
screenshots.

## Local checks

Run from the repository root with GNU Make and a host C++17 compiler:

```sh
make -C examples/four-line-command-core test
```

The test runner covers independent set/clear/scroll operations for all four
lines, malformed and unsupported inputs, range checks, state immutability after
rejection, and concurrent snapshots. It has no network, hardware, embedded
toolchain, or production-source dependency. Clean its generated objects with:

```sh
make -C examples/four-line-command-core clean
```

The redacted root entry point is checked by source review rather than by a
successful build. `src/main.cpp` contains an explicit compile-time `#error`; a build
stopping there is the intended safety boundary. For a quick local guard check:

```sh
grep -n '^#error ' src/main.cpp
```

| ID | Check | Expected local result | Status |
| --- | --- | --- | --- |
| L1 | Host-side four-line command core | The test runner exits zero after checking line-local state, validation, and mutex snapshots. | Can run locally; not production evidence |
| L2 | Redacted source compile-time guard | `grep` finds the intentional `#error` in `src/main.cpp`; compiling the source is expected to stop. | Can run locally; not a firmware build |

## Hardware and production cases

The device context is an ESP32 with W5500 wired Ethernet and a four-line P10
display. The P10 test board had some non-working pixels; record whether a visible
problem follows a physical location or a logical line before attributing it to
software. Do not use the host example's passing result as evidence for any row
below.

**Status legend:** Media evidence (captured by me; owner permission granted)
means visual evidence exists but is not a complete causal test record. Implemented,
deployment details withheld means the behavior is represented in documented
project history/source boundaries while deployment evidence stays private. Open
question means the public repository does not establish the behavior or outcome.

| ID | Area | Procedure and expected observation | Status |
| --- | --- | --- | --- |
| H1 | Boot | Power-cycle the device and observe stable startup without repeated resets over an agreed interval. Repeat after a second cycle. | Implemented, deployment details withheld |
| H2 | Ethernet | On an isolated test LAN, observe W5500 link initialization and expected connectivity. | Implemented, deployment details withheld |
| H3 | TCP | Submit a sanitized synthetic request and check connection establishment, reception, and lifecycle handling. | Implemented, deployment details withheld |
| H4 | Device parsing | At the project device boundary, compare accepted synthetic input with malformed JSON, missing fields, wrong types, and oversized input. The production schema is not published. | Implemented, deployment details withheld |
| H5 | TTL/UART development path | TTL/UART was used during development for controller communication; packet contents, serial settings, adapter details, and deployment exposure remain withheld. | Implemented, deployment details withheld |
| H6 | Four-line display | Set, clear, and position one line at a time; verify that the other three independently controlled lines remain unchanged. Separate non-working P10 pixels from logical-line behavior. | Media evidence (captured by me; owner permission granted) |
| H7 | Scrolling | Check fixed and scrolling content, start/stop behavior, and boundaries without asserting an undocumented timing target. | Media evidence (captured by me; owner permission granted) |
| H8 | Font/style variation | Compare captured states using different text presentation settings without inferring the complete font configuration. | Media evidence (captured by me; owner permission granted) |
| H9 | Persistence | Change a supported setting, restart, and compare the restored state with the expected result. | Implemented, deployment details withheld |
| H10 | Concurrency | Apply repeated command changes while rendering and look for inconsistent frames, cross-line changes, or stalled work. | Implemented, deployment details withheld |
| H11 | OTA implementation/evaluation | I implemented and evaluated SOTA and LAN OTA; I selected LAN OTA for the W5500-connected installation. Operational details and public deployment evidence remain withheld. | Implemented, deployment details withheld |
| H12 | OTA failure handling | Interrupt a test upload or provide an invalid image; detailed recovery evidence is retained in the linked evidence archive. | Implemented, deployment details withheld |

Manual RealTerm packet sending was development tooling. The UART link is separate
from the W5500/TCP path; packet contents, serial settings, adapter details, and
production exposure remain withheld. Local photos captured by me and the linked
Drive video archive document the test-display states and broader project
evidence.

## Cause-and-effect capture plan

**Status: Implemented, deployment details withheld.** The linked
Google Drive archive contains the broader hardware and command-validation
evidence. The public repository does not reproduce command contents, terminal
settings, deployment identifiers, or the full cause/effect recording.

Passing any one case would not establish security, production suitability, or
comprehensive coverage. The OTA design and open questions are documented in
[docs/ota-design.md](../docs/ota-design.md); the evidence policy is in
[docs/testing.md](../docs/testing.md).
