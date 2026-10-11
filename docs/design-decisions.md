# Design decisions

This page records the project at a documentation-first level. It captures the direction reported for the controller without exposing production source, internal protocols, deployment details, or other proprietary implementation information.

## Decision summary

| Topic | Direction | Rationale and boundary |
| --- | --- | --- |
| Wired connectivity | Use an ESP32 with a W5500 Ethernet controller over SPI | The controller is intended for a wired Ethernet environment. Exact board wiring and production configuration are intentionally out of scope. |
| Display responsibilities | Treat display state, layout, and refresh as distinct concerns | This keeps timing-sensitive display work conceptually separate from slower command and configuration work. The description is not a source-code trace. |
| Persistent settings | Use the ESP32 Preferences/NVS capability for supported settings | Persistence is part of the system boundary; the supported key set, migration behavior, and recovery details are withheld. |
| OTA direction | The project owner reports that both SOTA and LAN OTA approaches were implemented and evaluated; LAN OTA was selected | LAN OTA was reported as the better fit for this Ethernet-connected controller. The repository does not define the protocol, authentication, authorization, rollback, or deployment process, and does not claim production readiness. |
| Public documentation | Keep the repository conceptual and release-reviewed | The project record can explain engineering boundaries without publishing production firmware, credentials, internal endpoints, or unapproved evidence. |

## Interpretation

These are high-level project decisions and reported outcomes, not independent verification. The OTA decision should be read together with the [README OTA status](../README.md#ota-status), and any public attribution or implementation detail requires project-owner review.

The [system overview image](../media/system-overview.png) and editable diagrams provide visual context only. They should not be used as wiring instructions or as a substitute for the withheld production implementation.

## Disclosure boundary

Production source, build configuration, firmware binaries, internal network identifiers and endpoints, credentials, logs, exact command schemas, and unapproved test evidence remain outside this repository. Licensing remains unspecified pending authorization.
