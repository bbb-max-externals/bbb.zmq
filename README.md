# bbb.zmq

> [!WARNING]
> This repository is published as AI-assisted, insufficiently tested work in progress ("AI slop"). Treat it as experimental. Correctness, stability, compatibility, and fitness for production use are not guaranteed.

Max/MSP external package for ZeroMQ integration with binary payload parsing via a small DSL.

## Object Set

```text
bbb.zmq.recv       Receive ZMQ messages, store frames, output packet handles
bbb.zmq.send       Send ZMQ messages with type-aware frame encoding
bbb.zmq.route      Route by first frame text key; consumes matched frame
bbb.zmq.routepass  Route by first frame text key; preserves matched frame
bbb.zmq.schema     Load/compile/register DSL schemas
bbb.zmq.parse      Parse packet frame using compiled schema
bbb.zmq.peek       Debug inspector for packet views
```

## Core Idea

Patch cords carry lightweight handles, not raw binary data:

```text
packet 12345
```

The actual ZMQ message frames live in an internal C++ runtime (`PacketStore`). This allows Max-style patching while keeping binary handling efficient.

## Typical Receive Patch

```text
[bbb.zmq.recv @endpoint tcp://*:5555 @type sub]
|
[bbb.zmq.route /imu /camera /status]
|
[bbb.zmq.parse imu]
|
[route timestamp accel gyro]
```

## Typical Send Patch

```text
[42 3.14] → [bbb.zmq.send @endpoint tcp://*:5556 @type pub]
```

`bbb.zmq.send` encodes atoms by type:

| Max type | Encoding | Size |
|----------|----------|------|
| int/long | int64 (BE/LE) | 8 bytes |
| float | double IEEE 754 (BE/LE) | 8 bytes |
| symbol | raw UTF-8 bytes | variable |

Byte order controlled by `@endian big` (default) or `@endian little`.

## Build

```bash
cmake -B build -G Xcode
cmake --build build --config Release
```

Outputs to `externals/`:
- `bbb.zmq.recv.mxo`, `bbb.zmq.send.mxo`, `bbb.zmq.route.mxo`, `bbb.zmq.routepass.mxo`
- `bbb.zmq.schema.mxo`, `bbb.zmq.parse.mxo`, `bbb.zmq.peek.mxo`
- `libbbb_zmq_runtime.dylib` (shared runtime for singletons)

## Project Structure

```text
source/
  runtime/            shared runtime library (PacketStore, SchemaRegistry, DSL, ParserVM)
  projects/           one subdir per external
extern/
  min-api/            Cycling '74 min-api (git submodule)
  max-sdk/            Max SDK (git submodule)
  cppzmq/             ZeroMQ C++ bindings (git submodule)
  libzmq/             libzmq (git submodule)
externals/            build output (.mxo + dylib)
help/                 maxhelp files
schemas/              sample .zmqdsl schema files
docs/                 auto-generated maxref XML
```

## Dependencies

All dependencies are git submodules:
- **min-api** — Cycling '74 modern C++ API for Max externals
- **max-sdk** — Max SDK headers
- **cppzmq** — ZeroMQ C++ bindings (header-only)
- **libzmq** — ZeroMQ C library (built as static)

No external package managers required.

## Schema DSL

Schemas describe binary frame layout:

```text
schema imu {
  endian little;
  u16 magic = 0xCAFE;
  u8  version;
  u64 timestamp;
  f32 accel[3];
  f32 gyro[3];
  emit timestamp timestamp;
  emit accel accel[0] accel[1] accel[2];
  emit gyro gyro[0] gyro[1] gyro[2];
}
```

Load from file or textedit:

```text
[textedit] → [prepend set] → [bbb.zmq.schema @name imu @autocompile 1]
```

See `SPEC.md` for the full DSL specification.

## Route vs Routepass

### `bbb.zmq.route` — consumes matched frame

```text
[bbb.zmq.route /sensor]
|
[bbb.zmq.route /imu]
|
[bbb.zmq.parse imu]
```

Each match strips the routing key. After two routes, frame[0] is the payload.

### `bbb.zmq.routepass` — preserves matched frame

```text
[bbb.zmq.routepass /imu]
|
[bbb.zmq.parse imu @frame 1]
```

Downstream objects still see the routing key at frame[0].

## bbb.zmq.send Messages

| Message | Description |
|---------|-------------|
| `start` / `bang` | Create socket and start send thread |
| `stop` | Stop send thread and cleanup |
| `send <atoms>` | Encode atoms into one frame, send immediately |
| `frame <atoms>` | Add type-aware frame to multipart buffer |
| `frame_bytes <ints>` | Add raw byte frame (each int 0-255) to buffer |
| `flush` | Send buffered frames as multipart, clear buffer |

Multipart example:

```text
[frame sensor]     \
[frame 42 3.14]     → [bbb.zmq.send]
[flush]            /
```

## bbb.zmq.recv Attributes

| Attribute | Default | Description |
|-----------|---------|-------------|
| `@endpoint` | `tcp://*:5555` | ZMQ endpoint |
| `@type` | `sub` | Socket type: sub, pull, rep, router, pair |
| `@bind` | `1` | 1=bind, 0=connect |
| `@subscribe` | `""` | Subscription topic (sub sockets) |
| `@hwm` | `1000` | Receive high water mark |

## bbb.zmq.send Attributes

| Attribute | Default | Description |
|-----------|---------|-------------|
| `@endpoint` | `tcp://*:5556` | ZMQ endpoint |
| `@type` | `pub` | Socket type: pub, push, pair |
| `@bind` | `1` | 1=bind, 0=connect |
| `@hwm` | `1000` | Send high water mark |
| `@endian` | `big` | Byte order for numeric encoding: big or little |

## Loopback Self-Test

The test patch (`bbb.zmq.test.maxpat`) is configured for self-testing:

```text
[bbb.zmq.send @endpoint tcp://*:5556 @type pub]   (binds)
[bbb.zmq.recv @endpoint tcp://localhost:5556 @bind 0 @type sub]  (connects)
```

1. Start sender, then start receiver
2. Click `send sensor 42 3.14` — loopback through ZMQ
3. Route → parse → see decoded output

## Dependencies

| Library | License | Notes |
|---------|---------|-------|
| [libzmq](https://github.com/zeromq/libzmq) | MPL-2.0 | ZeroMQ C library (built as static) |
| [cppzmq](https://github.com/zeromq/cppzmq) | MIT | ZeroMQ C++ bindings (header-only) |
| [min-api](https://github.com/Cycling74/min-api) | MIT | Cycling '74 modern C++ API for Max externals |
| [max-sdk-base](https://github.com/Cycling74/max-sdk-base) | Cycling '74 EULA | Max SDK headers and libs |

## What v0.1 Does Not Include Yet

```text
if / switch
struct / nested struct
field-level endian override
bitfields
alignment directives
checksum validation
multi-frame DSL blocks
matrix output
dict output
JSON decoding
schema-aware routing
file watching
prefix route mode
bbb.zmq.encode (standalone binary encoder)
req/dealer bidirectional sockets
```

See `SPEC.md` for the full specification.
