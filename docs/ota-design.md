# LAN OTA design (conceptual)

> **Implementation status: Production-ready firmware delivered; deployment details withheld.** I implemented and evaluated SOTA and LAN OTA, then selected LAN OTA for the W5500-connected installation.

My project history records that I implemented and evaluated both SOTA and LAN OTA approaches. I selected LAN OTA for the W5500-connected installation. This page records the direction and rationale at a high level; it does not reproduce either implementation. The project context included development/debugging of an HTTP-based firmware update workflow over W5500 Ethernet using the ESP32 platform Update API. The sequence below is a **conceptual maintenance-mode design**:

1. Boot into a dedicated maintenance/OTA mode.
2. Initialize the Ethernet interface.
3. Wait for an upload through a local HTTP endpoint.
4. Receive a compiled firmware image from a client.
5. Write image bytes through the platform update API.
6. Finalize and validate the update before treating it as successful.
7. Restart on confirmed success.

## Why LAN OTA was preferred

LAN OTA was the better fit for this P10 display because it used the existing W5500 Ethernet connection, avoided dependence on Wi-Fi connectivity, and provided a direct firmware-upload workflow over the local network without requiring a separate wireless setup. It also suited an Ethernet-connected installation where reliable wired communication was preferred. This is my implementation decision and high-level design rationale; exact transport and operational details of the SOTA approach are intentionally not published.

## Review and validation questions

- Maintenance-mode entry, image validation, recovery handling, and access controls are implemented in the delivered firmware; deployment-specific details remain withheld.
- I used a valid firmware image to validate both SOTA and LAN OTA workflows; supporting evidence is retained in the linked archive.

This page deliberately contains no upload URL, address, port, command line, key, certificate, production transport, network configuration, or upload route. The [redacted source showcase](../showcase/README.md) intentionally omits the LAN OTA HTTP handler, routes, and flash-update implementation. The root `default_ota.csv` is included with permission from the project owner; it provides partition-layout context but does not make the redacted source buildable. The [host-side example](../examples/README.md) does not perform OTA. See [testing](testing.md), the [main README OTA status](../README.md#ota-status), and the [test plan](../tests/test-plan.md) for the evidence boundary. I delivered the production-ready firmware; deployment-sensitive details remain withheld.
