# Networking and command boundaries

> Generic processing model. This page does not define the production protocol or network configuration.

The documented network path is TCP over wired Ethernet supplied by a W5500 controller connected to the ESP32 over SPI. JSON is the structured command representation at the processing boundary. A separate serial/UART path was used during development to send command packets by hand; it is not presented as a second production transport. See the [main README](../README.md#overview) for the project summary and [hardware](hardware.md) for the interface boundary.

1. A client connects to a configured TCP service on the controller through the W5500 wired Ethernet interface.
2. The controller receives a command payload; message size and completeness must be considered before parsing.
3. JSON is parsed and validated for structure, field types, supported operations, and acceptable values.
4. An accepted operation is dispatched to display/configuration state through an appropriate synchronization boundary.
5. A response or status may be returned, depending on the implementation; no response schema is published.

The separation between transport, parsing, validation, and state updates helps reason about malformed input and concurrent rendering. A TCP connection by itself does not authenticate a client or guarantee that the command payload is safe. Details of framing, responses, access control, network configuration, and failure behavior require project documentation and testing. Supported display state may be retained through Preferences/NVS, but production network/configuration persistence is intentionally not described.

### Illustrative payload only

The following invented payload illustrates JSON shape **only**. It is not a production command, schema, endpoint, or working example for the device.

```json
{
  "cmd": "example",
  "message": "DEMO"
}
```

No actual addresses, MACs, ports, hostnames, command names, or internal endpoints are disclosed. See [architecture](architecture.md) for the component relationship and [test planning](../tests/test-plan.md) for unverified parsing/network cases. The runnable [host-side example](../examples/README.md) demonstrates only a limited, independently written command boundary; it is not the device protocol.

### Serial command path

Alongside the TCP path described above, command packets were also sent to the controller over its serial/UART link during development and testing. That link is a distinct interface from the W5500 Ethernet path; its framing, responses, and access characteristics are not documented here, and how it is exposed outside development is not asserted. As with TCP, a physical or serial connection does not by itself authenticate a client. Manual RealTerm use is project history and tooling context, not a reproducible passed test; see the [test plan](../tests/test-plan.md) and [testing policy](testing.md).

## Release boundary

The [redacted source showcase](../showcase/README.md) intentionally omits the production TCP server, command schema, network configuration, operational management commands, and LAN OTA upload implementation. My captured media and project records support that W5500 Ethernet/TCP and TTL/UART communication were used during the project, while deployment details remain withheld. This page therefore describes interfaces and boundaries rather than an integration contract; the public release does not include production packet captures or network configuration. The display media is read alongside the linked command-validation evidence.
