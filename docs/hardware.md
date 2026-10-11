# Hardware roles and integration considerations

> Component-level overview only. It is not a schematic, assembly guide, or approved bill of materials.

| Component | Role |
| --- | --- |
| ESP32 | Runs controller firmware and coordinates network, state, and display work. |
| W5500 | Provides a wired Ethernet interface; its SPI connection to the ESP32 requires device selection and coordinated bus access. |
| P10 LED matrix panel(s) | Presents rendered display content; panel interface and arrangement depend on the actual hardware. |
| Power supply | Must be evaluated for the controller and display's real electrical requirements, startup behavior, and shared-ground arrangement. |

## Interfaces

The W5500 is an SPI peripheral. SPI transfers use clock/data signals and a chip-select concept to select a device. Any other use or sharing of SPI, including display wiring, needs hardware-specific confirmation; the [system diagram](../diagrams/system-architecture.mmd) intentionally does not specify it.

LED matrix refresh can be time-sensitive. Wiring, signal integrity, voltage compatibility, available current, and thermal behavior should be checked against the actual components before using a physical build. No GPIO mapping, board revision, panel count, power rating, or production pin assignment is published here.
