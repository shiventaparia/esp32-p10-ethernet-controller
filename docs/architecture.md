# Architecture (conceptual)

> This is a public, high-level model of responsibilities, not the production implementation, an exact boot sequence, or a wiring schematic.

The ESP32 is the central controller. A W5500 interface provides wired Ethernet and communicates with the controller over SPI. At the software boundary, a TCP-facing component receives requests; JSON interpretation and validation map supported requests to operations on display or configuration state. During development, command packets were also sent over a separate serial/UART path. Rendered state is presented on a four-line P10 LED matrix in which every line is driven independently. ESP32 Preferences/NVS can retain supported **display** state across restarts. FreeRTOS separates network-facing work from display work, while a timer-driven scan keeps the timing-sensitive refresh path distinct. This description deliberately omits protocol fields, pin assignments, network configuration, and implementation details.

This block is byte-identical to the editable source in `../diagrams/system-architecture.mmd`.

```mermaid
%% Portfolio evidence only: conceptual architecture, not exact wiring, topology, or a production source-code trace.
%% The root src/main.cpp is the only firmware source and is redacted/non-buildable; production network/OTA implementation and sensitive deployment values are omitted.
flowchart TD
    Client[LAN Client] --> TCP[TCP Server]
    Serial[Serial / UART<br/>(development path)] -. command packets .-> JSON[Command / JSON Processing]
    TCP --> JSON[Command / JSON Processing]
    JSON --> State[Display and Configuration State]
    State --> Render[Display Rendering]
    Render --> Lines[Four independently controlled display lines]
    Lines --> P10[P10 LED Matrix]
    State <--> NVS[Preferences / NVS]
    W5500[W5500 Ethernet] <--> TCP
    RTOS[FreeRTOS Tasks and Synchronization] -. coordinates .-> TCP
    RTOS -. coordinates .-> Render
    Timer[Display Refresh Timer / ISR] --> P10
```

[Editable Mermaid source](../diagrams/system-architecture.mmd) · [Conceptual firmware flow](../diagrams/firmware-flow.mmd)

The diagram focuses on the network path. A serial/UART link was also used to send command packets during development; it is described at a high level under [hardware](hardware.md) and [networking](networking.md).

## Responsibility boundaries

| Boundary | Design consideration |
| --- | --- |
| Wired interface → request handling | SPI connects the ESP32 and W5500; TCP supplies a transport, not a trusted command. |
| Serial link → request handling | The UART/serial path also carried command packets during development; its framing and exposure outside development are not documented here. |
| Request handling → state | Validate structure and supported operations before updating state; see [networking](networking.md). |
| State → persistence | Preferences/NVS is the public persistence boundary for supported display state; production network/configuration persistence is intentionally omitted. |
| State → display | Coordinate readers and writers so rendering observes consistent state; an update to one of the four lines must not disturb the other lines. See [concurrency](concurrency.md). |
| Rendering → physical refresh | Keep timer-driven, timing-sensitive scan work short and avoid network processing in that path. |

FreeRTOS tasks provide a way to separate network and display responsibilities; shared mutable state calls for synchronization rather than relying on task/core placement. The diagram expresses coordination, not a particular task count, core assignment, scheduling policy, bus topology for the display, or code-level call graph.

## Public implementation boundary

The redacted source showcase under [`showcase/`](../showcase/README.md) preserves selected display, persistence, mutex, FreeRTOS, and scan-boundary patterns for review. It intentionally omits the production transport, network/configuration implementation, operational command schema, and LAN OTA implementation; it is not a firmware build. The independently written host-side example in [`examples/README.md`](../examples/README.md) is the runnable illustration of four-line state and command handling, not embedded firmware.

These relationships remain conceptual and unverified in this repository. See the [main README](../README.md), the [test plan](../tests/test-plan.md), and [concurrency](concurrency.md) for the release boundary and validation status.

The root `src/main.cpp` is the only firmware source and is redacted/non-buildable.
The original root support files are included with permission from the project
owner, while `.pio`, binaries, generated build output, local-path IDE files,
production network/OTA implementation, and sensitive deployment values are
omitted. The host-side example remains an independent runnable illustration;
this document and its diagrams are portfolio evidence.
