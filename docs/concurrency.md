# Concurrency and display refresh

> Design considerations based on the project context; no exact task placement, priority, or production synchronization code is disclosed.

The controller has two timing domains to keep conceptually separate: FreeRTOS-managed network/command work and display work that maintains four independently controlled P10 lines. A per-line state change should not rewrite or destabilize the other lines. The public [architecture](architecture.md) diagram shows these responsibilities without asserting a particular production task graph.

A FreeRTOS task is a schedulable unit of work. Tasks assigned to separate ESP32 cores can execute in parallel. Separating network communication from display work can keep slower request handling out of the rendering path, but separation alone does not make shared data safe. The intended boundary is network task → validated state transition → display task, not a claim that the public repository contains those production tasks.

## Shared-state boundary

Network processing may propose a new message or setting while display work reads the current state. A mutex or semaphore can coordinate access to shared state; a mutex protects only the critical sections that consistently use it. All other readers/writers and compound operations still need a coherent ownership or synchronization strategy. Avoid holding a lock across lengthy network activity, Preferences/NVS operations, or timing-sensitive display operations; determine the actual locking/ownership design from the implemented project before documenting it as fact. The public source showcase shows selected mutex-protected display access and persistence boundaries, but it is intentionally non-buildable and is not production synchronization code.

## Timing boundary

Timer-driven display scanning or refresh has different timing requirements from client I/O and JSON processing. Interrupt service routines should do minimal work: avoid blocking, dynamic allocation, and lengthy processing in an ISR. Rendering, state management, Preferences/NVS access, and parsing belong in an appropriate task context rather than in a long-running interrupt handler. The showcase preserves this scan/ISR boundary, while exact timer wiring, scheduling guarantees, scan interval, and core pinning remain undisclosed and unverified.

Validation questions include simultaneous command updates and rendering, rapid per-line changes on the four-line display, persistence interactions, and responsiveness while the display refreshes. The display media captured by me provides **Media evidence (captured by me; owner permission granted)** for visible line/content states, while repeatable FreeRTOS scheduling, timer scanning, and hardware concurrency remain **Open question** in [the test plan](../tests/test-plan.md). The independently written [host-side example](../examples/README.md) can exercise mutex-protected snapshots and four-line state without validating FreeRTOS scheduling, timer scanning, or hardware. See the [main README](../README.md#validation-and-disclosure) and [showcase README](../showcase/README.md) for the evidence and disclosure boundaries.
