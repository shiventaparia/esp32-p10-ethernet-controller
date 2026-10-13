# Troubleshooting guide

This guide provides documentation-first checks for interpreting observations from the ESP32 P10 Ethernet controller. It does not reproduce production procedures or proprietary implementation details.

## Start with the observation

Record the visible symptom, the test setup, the input that preceded it, and whether the behavior is repeatable. Separate an observation from a conclusion: a display artifact, a missing network response, or an update failure does not identify its cause by itself.

## Display observations

- Check the P10 panel, connectors, power arrangement, and test setup before attributing a visible artifact to firmware.
- The P10 display board used for testing had some non-working pixels. A missing or incorrect-looking pixel may therefore be hardware-related and is not automatically a firmware defect.
- Compare a fixed message with a scrolling message and note whether the issue follows the content, position, or physical location on the panel.
- Record whether the issue affects the whole panel, one of the four independently controlled lines, or isolated pixels. This helps distinguish a test-hardware observation from a higher-level rendering concern without asserting a root cause.

## Ethernet and command observations

- Confirm that the controller and test client are using the intended wired network path.
- Record whether the issue is connection establishment, request handling, validation, or display-state application.
- Keep TCP observations separate from the development-only UART/RealTerm path; success on one path does not prove behavior on the other.
- Avoid treating a silent or rejected request as proof of a particular internal protocol behavior; the production command schema is not published here.

## Configuration and restart observations

- Note which supported setting was changed and whether the observation occurred before or after a restart.
- Distinguish a runtime change from persistence across restart. The repository identifies Preferences/NVS as a project boundary; the [showcase](../showcase/README.md) shows a generic display-state key set for review, while the production network/configuration key set and the boot sequence are not published.
- The host-side example can check state immutability and line-local behavior, but it cannot validate ESP32 NVS, timing, Ethernet, or panel behavior.

## OTA observations

I implemented and evaluated both SOTA and LAN OTA approaches, then selected LAN OTA as the better fit for this Ethernet-connected controller. This repository does not specify the update protocol, security controls, rollback behavior, or release process.

For any OTA report, record high-level evidence such as the approach evaluated, the observed outcome, and the test context. I delivered production-ready firmware and validated both OTA approaches; deployment-specific operational details remain in the linked archive.

## Escalation and disclosure

When an issue cannot be separated from hardware, network environment, or implementation behavior, preserve the observation and review it before publication. Do not add credentials, internal endpoints, proprietary logs, production source, or unpermitted media to this documentation.

For the public-source boundary, see [showcase/README.md](../showcase/README.md).
