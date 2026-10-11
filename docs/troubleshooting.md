# Troubleshooting guide

This guide provides documentation-first checks for interpreting observations from the ESP32 P10 Ethernet controller. It does not reproduce production procedures or proprietary implementation details.

## Start with the observation

Record the visible symptom, the test setup, the input that preceded it, and whether the behavior is repeatable. Separate an observation from a conclusion: a display artifact, a missing network response, or an update failure does not identify its cause by itself.

## Display observations

- Check the P10 panel, connectors, power arrangement, and test setup before attributing a visible artifact to firmware.
- The P10 display board used for testing had some non-working pixels. A missing or incorrect-looking pixel may therefore be hardware-related and is not automatically a firmware defect.
- Compare a fixed message with a scrolling message and note whether the issue follows the content, position, or physical location on the panel.
- Record whether the issue affects the whole panel, a logical row/region, or isolated pixels. This helps distinguish a test-hardware observation from a higher-level rendering concern without asserting a root cause.

## Ethernet and command observations

- Confirm that the controller and test client are using the intended wired network path.
- Record whether the issue is connection establishment, request handling, validation, or display-state application.
- Avoid treating a silent or rejected request as proof of a particular internal protocol behavior; the production command schema is not published here.

## Configuration and restart observations

- Note which supported setting was changed and whether the observation occurred before or after a restart.
- Distinguish a runtime change from persistence across restart. The repository identifies Preferences/NVS as a project boundary but does not publish its key set or boot sequence.

## OTA observations

The project owner reports that both SOTA and LAN OTA approaches were implemented and evaluated, with LAN OTA selected as the better fit for this Ethernet-connected controller. This repository does not specify the update protocol, security controls, rollback behavior, or release process.

For any OTA report, record only approved high-level evidence such as the approach evaluated, the observed outcome, and the test context. Do not infer end-to-end production readiness from an upload, reboot, or a single successful observation.

## Escalation and disclosure

When an issue cannot be separated from hardware, network environment, or implementation behavior, preserve the observation and ask the project owner to review it. Do not add credentials, internal endpoints, proprietary logs, production source, or unapproved media to this documentation.
