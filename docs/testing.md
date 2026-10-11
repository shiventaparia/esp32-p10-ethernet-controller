# Testing and evidence policy

This documentation-only repository does not contain firmware, hardware setup, logs, screenshots, or reproducible results. The table is a proposed validation checklist, **not evidence of execution**. Each status must remain unverified until the project owner confirms a test, its environment, and a release-safe description of the result. The detailed [test plan](../tests/test-plan.md) expands these checks without inventing outcomes.

| Area | Suggested test | Status |
| --- | --- | --- |
| Boot | Device starts without repeated resets | Needs confirmation |
| Ethernet | Link and network connectivity established | Needs confirmation |
| TCP | Client connection and request reception | Needs confirmation |
| Parsing | Valid and malformed payload handling | Needs confirmation |
| Display | Fixed text and positioning | Needs confirmation |
| Scrolling | Scrolling behavior and boundaries | Needs confirmation |
| Rows | Independent row update/erase behavior | Needs confirmation |
| Persistence | Supported settings survive restart | Needs confirmation |
| Concurrency | Shared state remains consistent under concurrent activity | Needs confirmation |
| OTA | Valid firmware upload and successful reboot | Needs confirmation |
| OTA failure handling | Interrupted/invalid upload does not falsely report success | Needs confirmation |

Manual command testing during development used RealTerm's packet-send feature over the controller's serial/UART link; see the [tooling note](../README.md#development-and-test-tooling). Driving a tool by hand is not a reproducible test and does not by itself change any status above, and none of the rows above cover that serial path.

When updating evidence later, first obtain the owner's approval to share it, then record the tested setup, procedure, expected/observed behavior, and date without revealing identifiers or proprietary details. In particular, OTA remains **Needs confirmation from the project owner**; development activity is not an end-to-end result.
