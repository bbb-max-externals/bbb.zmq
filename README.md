# bbb.zmq Max Parser DSL

Draft design for a Max/MSP package named `bbb.zmq.*`.

The package is intended for receiving ZeroMQ packets in Max, routing multipart frames in a Max-like way, and parsing binary payloads into normal Max messages using a small DSL.

## Object Names

```text
bbb.zmq.recv
bbb.zmq.route
bbb.zmq.routepass
bbb.zmq.schema
bbb.zmq.parse
bbb.zmq.peek
```

Core objects:

```text
bbb.zmq.recv       Receive ZMQ messages and store frames internally
bbb.zmq.route      Route by current first frame and consume it on match
bbb.zmq.routepass  Route by current first frame without consuming it
bbb.zmq.schema     Load/compile/register parser DSL schemas
bbb.zmq.parse      Parse current packet view frame using a schema
```

## Core Idea

Do not send large binary payloads through Max atom lists.

Instead, patch cords carry lightweight handles:

```text
packet 12345
```

The actual multipart frames remain in an internal runtime.

This allows Max-style patching while keeping binary handling efficient.

## Typical Patch

ZMQ multipart message:

```text
frame[0] = "/imu"
frame[1] = <binary imu payload>
```

Max patch:

```text
[bbb.zmq.recv @endpoint tcp://*:5555 @type sub]
|
[bbb.zmq.route /imu]
|
[bbb.zmq.parse imu]
|
[route timestamp accel gyro]
```

Because `bbb.zmq.route` consumes the matched frame, `bbb.zmq.parse` reads the payload as current `frame[0]`.

## Route vs Routepass

### `bbb.zmq.route`

Consumes the matched frame.

Input view:

```text
frame[0] = "/sensor"
frame[1] = "/imu"
frame[2] = <payload>
```

Patch:

```text
[bbb.zmq.route /sensor]
|
[bbb.zmq.route /imu]
|
[bbb.zmq.parse imu]
```

By the time the packet reaches `bbb.zmq.parse`, the current first frame is the binary payload.

### `bbb.zmq.routepass`

Matches but does not consume.

```text
[bbb.zmq.routepass /imu]
|
[bbb.zmq.parse imu @frame 1]
```

Use this when downstream objects still need the route key frame.

## Parser DSL

The DSL describes the byte layout inside one frame.

It does not specify which ZMQ frame to read. Frame selection belongs to `bbb.zmq.parse`.

Example schema:

```text
schema imu {
  endian little;
  encoding utf8;
  onerror error;

  u16 magic = 0xCAFE;
  u8  version;
  u8  type;
  u64 timestamp;

  f32 accel[3];
  f32 gyro[3];

  emit timestamp timestamp;
  emit type type;
  emit accel accel[0] accel[1] accel[2];
  emit gyro gyro[0] gyro[1] gyro[2];
}
```

Output:

```text
timestamp 123456789
type 1
accel 0.12 -0.03 9.81
gyro 0.01 0.02 0.00
```

## Variable-length Payloads

Use a previously parsed unsigned integer field as a length.

```text
schema blob {
  endian little;
  encoding utf8;
  onerror error;

  u32 payload_len;
  bytes[payload_len] payload;

  emit payload payload@handle;
}
```

Large binary fields should normally be emitted as handles, not lists.

## Strings

Strings are C-style by default.

Null-terminated string:

```text
string name;
```

Fixed-width C string:

```text
string[32] device_name;
```

Rules:

```text
string name;      reads until 0x00
string[32] name;  reads exactly 32 bytes, using bytes before first 0x00 as content
```

Length-prefixed strings are modeled as bytes plus an emit modifier:

```text
u32 name_len;
bytes[name_len] name_raw;
emit name name_raw@string;
```

Or C-string interpretation:

```text
emit name name_raw@cstring;
```

## File-based Schema Workflow

Recommended production workflow:

```text
[bbb.zmq.schema @file schemas/imu.zmqdsl]
[bbb.zmq.parse imu]
```

If `schemas/imu.zmqdsl` contains:

```text
schema imu {
  endian little;
  u32 seq;
  emit seq seq;
}
```

then the schema is registered as `imu`.

## Patch-embedded Codebox/Textedit Workflow

For prototyping, keep the schema inside the Max patch:

```text
[textedit]
|
[prepend set]
|
[bbb.zmq.schema @name imu @autocompile 1]

[bbb.zmq.parse imu]
```

Manual compile version:

```text
[textedit]
|
[prepend set]
|
[bbb.zmq.schema @name imu]

[button]
|
[message compile]
|
[bbb.zmq.schema @name imu]
```

Supported `bbb.zmq.schema` messages:

```text
read <path>
reload
set <dsl text>
append <dsl line>
clear
compile
dump
write <path>
```

## Example: IMU

ZMQ multipart:

```text
frame[0] = "/imu"
frame[1] = binary payload
```

Patch:

```text
[bbb.zmq.recv @endpoint tcp://*:5555 @type sub]
|
[bbb.zmq.route /imu]
|
[bbb.zmq.parse imu]
|
[route timestamp type accel gyro quat]
```

Schema:

```text
schema imu {
  endian little;
  encoding utf8;
  onerror error;

  u16 magic = 0xCAFE;
  u8  version;
  u8  type;
  u64 timestamp;

  f32 accel[3];
  f32 gyro[3];
  f32 quat[4];

  emit timestamp timestamp;
  emit type type;
  emit accel accel[0] accel[1] accel[2];
  emit gyro gyro[0] gyro[1] gyro[2];
  emit quat quat[0] quat[1] quat[2] quat[3];
}
```

## Example: Named Blob

Layout:

```text
null-terminated name
u32 payload_len
payload bytes
```

Schema:

```text
schema named_blob {
  endian little;
  encoding utf8;
  onerror error;

  string name;
  u32 payload_len;
  bytes[payload_len] payload;

  emit name name;
  emit payload payload@handle;
}
```

## Example: Fixed-width C Strings

C-like layout:

```c
char topic[16];
char device[32];
uint32_t payload_len;
uint8_t payload[payload_len];
```

Schema:

```text
schema device_info {
  endian little;
  encoding utf8;
  onerror error;

  string[16] topic;
  string[32] device;
  u32 payload_len;
  bytes[payload_len] payload;

  emit topic topic;
  emit device device;
  emit payload payload@handle;
}
```

## Safety Limits

Recommended limits:

```text
maxbytes 1048576;
maxitems 4096;
maxatoms 1024;
maxstring 4096;
```

These can be placed in the schema or overridden on `bbb.zmq.parse`:

```text
[bbb.zmq.parse imu @maxbytes 1048576 @maxatoms 1024]
```

## What v0.1 Does Not Include Yet

Deferred features:

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
```

See `SPEC.md` for the full draft specification.
