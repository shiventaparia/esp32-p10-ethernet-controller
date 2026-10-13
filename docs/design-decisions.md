# Design decisions

This page records the project at a documentation-first level. It captures the direction reported for the controller without exposing production source, internal protocols, deployment details, or other proprietary implementation information.

## Decision summary

| Topic | Direction | Rationale and boundary |
| --- | --- | --- |
| Wired connectivity | Use an ESP32 with a W5500 Ethernet controller over SPI | The controller is intended for a wired Ethernet environment. Exact board wiring and production configuration are intentionally out of scope. |
| Display responsibilities | Treat display state, layout, and refresh as distinct concerns | Four logical display lines can be updated independently while timing-sensitive refresh work remains separate from slower command and configuration work. The description is not a source-code trace. |
| Persistent settings | Use the ESP32 Preferences/NVS capability for supported settings | Persistence is part of the system boundary; the [source showcase](../showcase/README.md) shows a generic display-state key set at review level, while the production network/configuration key set, migration behavior, and recovery details are withheld. |
| Development command path | Use a separate serial/UART path for manual command exercise during development | RealTerm packet-send use provided a practical development tool path. Packet contents, serial settings, adapter details, and any production exposure are intentionally unpublished; this is not a second production protocol claim. |
| OTA direction | I implemented and evaluated both SOTA and LAN OTA approaches and selected LAN OTA | LAN OTA was the better fit because it used the existing W5500 Ethernet connection, avoided dependence on Wi-Fi connectivity, provided a simpler direct firmware-upload workflow over the local network without a separate wireless setup, and suited an Ethernet-connected installation where reliable wired communication was preferred. Deployment-specific protocol, authentication, authorization, rollback, and operations remain withheld. |
| Public documentation | Keep the repository conceptual and release-reviewed | The project record can explain engineering boundaries without publishing production firmware, credentials, internal endpoints, or unpermitted evidence. |

## Interpretation

These are high-level project decisions and reported outcomes. The OTA decision
should be read together with the [README OTA status](../README.md#ota-status),
and deployment-specific implementation detail remains withheld.

The [system overview image](../media/system-overview.png) and editable diagrams provide visual context only. They should not be used as wiring instructions or as a substitute for the withheld production implementation.

The [redacted source showcase](../showcase/README.md) preserves selected display,
NVS, mutex, FreeRTOS, and scan-boundary patterns. The independent
[host-side example](../examples/README.md) provides the runnable four-line
state/command demonstration; neither artifact is the production protocol or a
reproducible firmware build.

## Disclosure boundary

Production source, production network/OTA implementation, sensitive deployment values, firmware binaries, generated build output, local-path IDE files, internal network identifiers and endpoints, credentials, logs, exact command schemas, the client's identity and engagement details, and unpermitted test evidence remain outside this repository. The original root support files (`platformio.ini`, `default_ota.csv`, `include/README`, `lib/README`, and `test/README`) are included with permission from the project owner; they are context files and do not make the redacted root source buildable. The development context — an internship at HashStudioz Technologies and delivery into a client project — is stated at a high level only, as recorded in the [README](../README.md#project-background). Licensing remains unspecified pending authorization.
