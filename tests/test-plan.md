# Proposed validation plan

> **All statuses: Needs confirmation.** No checks below have been verified in this public repository. OTA end-to-end success specifically needs confirmation from the project owner.

Use a release-approved test setup and synthetic inputs. For each case, record preconditions, steps, expected and observed behavior, date, and owner-confirmed outcome in a sanitized form before changing a status. Avoid publishing serial captures, network settings, packet contents, binary images, or internal screenshots.

Manual command testing during development sent packets with RealTerm (see the [tooling note](../README.md#development-and-test-tooling)). Driving a tool by hand is not a reproducible test, so it does not by itself change any status below and no packet contents are published here.

| Area | Suggested test | What to check | Status |
| --- | --- | --- | --- |
| Boot | Device starts without repeated resets | Observe stable startup over an agreed interval and repeat after a power cycle. | Needs confirmation |
| Ethernet | Link and network connectivity established | Observe link initialization and expected connectivity on an approved isolated LAN. | Needs confirmation |
| TCP | Client connection and request reception | Submit a synthetic request and check that connection lifecycle and reception are handled. | Needs confirmation |
| Parsing | Valid and malformed payload handling | Compare accepted synthetic input with invalid JSON, missing fields, and oversized input. | Needs confirmation |
| Display | Fixed text and positioning | Check placement and clearing against an agreed display layout. | Needs confirmation |
| Scrolling | Scrolling behavior and boundaries | Check start, stop, and edge cases without asserting an undocumented timing target. | Needs confirmation |
| Rows | Independent row update/erase behavior | Change, clear, or scroll one of the four display lines and check that the remaining lines are unaffected. | Needs confirmation |
| Persistence | Supported settings survive restart | Change an approved setting, restart, and compare restored state with expectation. | Needs confirmation |
| Concurrency | Shared state remains consistent under concurrent activity | Exercise repeated command changes while rendering; look for inconsistent frames or stalled work. | Needs confirmation |
| OTA | Valid firmware upload and successful reboot | With an authorized test image, confirm finalization and boot into the intended image. | Needs confirmation |
| OTA failure handling | Interrupted/invalid upload does not falsely report success | Interrupt a test upload and reject an invalid image; inspect recovery without claiming it succeeds. | Needs confirmation |

Passing any one case would not establish security, production suitability, or comprehensive coverage. The OTA design and open questions are documented in [docs/ota-design.md](../docs/ota-design.md); the evidence policy is in [docs/testing.md](../docs/testing.md).
