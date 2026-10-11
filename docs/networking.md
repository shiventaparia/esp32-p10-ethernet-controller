# Networking and command boundaries

> Generic processing model. This page does not define the production protocol or network configuration.

1. A client connects to a configured TCP service on the controller through the wired Ethernet interface.
2. The controller receives a command payload; message size and completeness must be considered before parsing.
3. JSON is parsed and validated for structure, field types, supported operations, and acceptable values.
4. An accepted operation is dispatched to display/configuration state through an appropriate synchronization boundary.
5. A response or status may be returned, depending on the implementation.

The separation between transport, parsing, validation, and state updates helps reason about malformed input and concurrent rendering. A TCP connection by itself does not authenticate a client or guarantee that the command payload is safe. Details of framing, responses, access control, and failure behavior require owner-approved documentation and testing.

### Illustrative payload only

The following invented payload illustrates JSON shape **only**. It is not a production command, schema, endpoint, or working example for the device.

```json
{
  "cmd": "example",
  "message": "DEMO"
}
```

No actual addresses, MACs, ports, hostnames, command names, or internal endpoints are disclosed. See [architecture](architecture.md) for the component relationship and [test planning](../tests/test-plan.md) for unverified parsing/network cases.

### Serial command path

Alongside the TCP path described above, command packets were also sent to the controller over its serial/UART link during development and testing. That link is a distinct interface from the W5500 Ethernet path; its framing, responses, and access characteristics are not documented here, and how it is exposed outside development is not asserted. As with TCP, a physical or serial connection does not by itself authenticate a client.
