# Concurrency and display refresh

> Design considerations based on the project context; no exact task placement, priority, or production synchronization code is disclosed.

A FreeRTOS task is a schedulable unit of work. Tasks assigned to separate ESP32 cores can execute in parallel. Separating network communication from display work can keep slower request handling out of the rendering path, but separation alone does not make shared data safe.

## Shared-state boundary

Network processing may propose a new message or setting while display work reads the current state. A mutex or semaphore can coordinate access to shared state; a mutex protects only the critical sections that consistently use it. All other readers/writers and compound operations still need a coherent ownership or synchronization strategy. Avoid holding a lock across lengthy network activity or timing-sensitive display operations; determine the actual locking/ownership design from the approved implementation before documenting it as fact.

## Timing boundary

Timer-driven display scanning or refresh has different timing requirements from client I/O and JSON processing. Interrupt service routines should do minimal work: avoid blocking, dynamic allocation, and lengthy processing in an ISR. Rendering, state management, and parsing belong in an appropriate task context rather than in a long-running interrupt handler. Actual timer wiring, scheduling guarantees, and core pinning are not asserted here.

Validation questions include simultaneous command updates and rendering, rapid per-line changes on the four-line display, and responsiveness while the display refreshes. These are planned checks, not passed tests; see [the test plan](../tests/test-plan.md).
