# Hardware roles and integration considerations

> Component-level overview only. It is not a schematic, assembly guide, or bill of materials.

The public project description is for an ESP32 controller with a W5500 Ethernet controller over SPI and a four-line P10 display. The four display lines are independently controlled at the logical display boundary; the physical panel arrangement, wiring, and production board details are intentionally withheld. See the [main README](../README.md) for the overall scope and the [redacted showcase boundary](../showcase/README.md) before treating any source-like material as hardware documentation.

| Component | Role |
| --- | --- |
| ESP32 | Runs controller firmware and coordinates network, state, and display work. |
| W5500 | Provides a wired Ethernet interface over SPI; its connection to the ESP32 requires device selection and coordinated bus access. |
| P10 LED matrix, four independently controlled lines | Presents rendered display content as four lines, each of which can be updated on its own. The physical panel arrangement behind those lines is not published. |
| UART / serial link | Separate development path used to send command packets while exercising command handling; serial settings, adapter details, and non-development exposure are not published. |
| Custom controller PCB | A project-specific board was made and used to attach the W5500 Ethernet hardware to the controller system; the video I captured provides visual evidence of the test board. The schematic and manufacturing files are not published here. |
| Power supply | Must be evaluated for the controller and display's real electrical requirements, startup behavior, and shared-ground arrangement. |

## Interfaces

The W5500 is an SPI peripheral. SPI transfers use clock/data signals and a chip-select concept to select a device. Any other use or sharing of SPI, including display wiring, needs hardware-specific confirmation; the [system diagram](../diagrams/system-architecture.mmd) intentionally does not specify it. The W5500 connection is the hardware basis for the documented TCP/LAN path, but this page does not publish a pin map or network configuration.

The ESP32's UART/serial link is a separate path from the W5500 interface and was used during development to send command packets to the controller by hand (see the [tooling note](../README.md#development-and-test-tooling)). The serial parameters, the adapter used, and the packet format are not published here, and how that path is exposed outside development is not asserted. The [networking page](networking.md) describes both paths without reproducing the production transport.

LED matrix refresh can be time-sensitive. Wiring, signal integrity, voltage compatibility, available current, and thermal behavior should be checked against the actual components before using a physical build. No GPIO mapping, board revision, physical panel count, power rating, or production pin assignment is published here.

## Display configuration

The matrix presents four lines of content, and every line is controlled independently, so one line can be changed, cleared, or scrolled without rewriting the others. This four-line arrangement was part of the implemented project; how those lines map onto physical P10 panels, and how the panels are chained and powered, depends on the actual build and is not published here.

## Custom PCB evidence

The custom controller PCB was made and used as part of the project hardware
integration. The attached W5500 hardware is visible in the photos and videos I captured in the
[Google Drive hardware evidence archive](https://drive.google.com/drive/folders/1Rl7MxlMSVG_yuMZJBnGRHxVEYuInktFD?usp=sharing). The project owner gave permission to share this evidence.
This is visual evidence that the board was used in the test setup; it is not a
published schematic, PCB layout, manufacturing release, electrical
qualification, or exact pin assignment.

## Test-board caveat

The P10 display board used for testing had some non-working pixels. A missing or incorrect-looking pixel is therefore not automatically a firmware defect. Interpret display observations alongside the condition of the test hardware and record whether an issue follows a physical location or one of the four logical lines; see [troubleshooting](troubleshooting.md) and the [test plan](../tests/test-plan.md). The four-line, scrolling, and font/style rows have **Media evidence (captured by me; owner permission granted)**; electrical, pixel-health, and repeatable hardware behavior remain **Open question**. The host-side [example](../examples/README.md) can exercise state and command boundaries without proving hardware behavior.
