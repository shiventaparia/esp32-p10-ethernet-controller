# Hardware roles and integration considerations

> Component-level overview only. It is not a schematic, assembly guide, or approved bill of materials.

| Component | Role |
| --- | --- |
| ESP32 | Runs controller firmware and coordinates network, state, and display work. |
| W5500 | Provides a wired Ethernet interface; its SPI connection to the ESP32 requires device selection and coordinated bus access. |
| P10 LED matrix, four independently controlled lines | Presents rendered display content as four lines, each of which can be updated on its own. The physical panel arrangement behind those lines is not published. |
| Power supply | Must be evaluated for the controller and display's real electrical requirements, startup behavior, and shared-ground arrangement. |

## Interfaces

The W5500 is an SPI peripheral. SPI transfers use clock/data signals and a chip-select concept to select a device. Any other use or sharing of SPI, including display wiring, needs hardware-specific confirmation; the [system diagram](../diagrams/system-architecture.mmd) intentionally does not specify it.

LED matrix refresh can be time-sensitive. Wiring, signal integrity, voltage compatibility, available current, and thermal behavior should be checked against the actual components before using a physical build. No GPIO mapping, board revision, physical panel count, power rating, or production pin assignment is published here.

## Display configuration

The matrix presents four lines of content, and every line is controlled independently, so one line can be changed, cleared, or scrolled without rewriting the others. This four-line arrangement is confirmed by the project owner; how those lines map onto physical P10 panels, and how the panels are chained and powered, depends on the actual build and is not published here.
