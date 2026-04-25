# bbb.zmq Max Parser DSL Specification

Version: v0.1-draft

This document defines the draft specification for a Max/MSP external package named `bbb.zmq.*`.

The package receives ZeroMQ multipart packets, routes them in a Max-like patching style, and parses binary payloads using a small domain-specific language.

The main design goal is to preserve the Max idiom of patch-cord routing while avoiding large binary payloads traveling through Max atom lists.

---

## 1. Object Set

The v0.1 object set:

```text
bbb.zmq.recv       ZeroMQ receiver
bbb.zmq.send       ZeroMQ sender (pub/push/pair)
bbb.zmq.route      Max-like multipart frame router; consumes matched frame
bbb.zmq.routepass  Max-like multipart frame router; does not consume matched frame
bbb.zmq.schema     DSL loader/compiler/registry object
bbb.zmq.parse      PacketView parser using compiled DSL schema
bbb.zmq.peek       Debugging object for inspecting packet frames/fields
```

Minimum implementation set:

```text
bbb.zmq.recv
bbb.zmq.route
bbb.zmq.routepass
bbb.zmq.schema
bbb.zmq.parse
```

---

## 2. Architecture Overview

The system is composed of four conceptual layers:

```text
ZMQ socket / receiver / sender
  ↓↑
PacketStore / PacketView runtime
  ↓
Max-style routing objects
  ↓
Parser DSL → Max messages
```

Patch cords carry lightweight packet view handles:

```text
packet <id>
```

The actual ZMQ message frames remain in an internal C++ runtime.

Typical receive patch:

```text
[bbb.zmq.recv @endpoint tcp://*:5555 @type sub]
|
[bbb.zmq.route /imu /camera /status]
|
[bbb.zmq.parse imu]
|
[route timestamp accel gyro]
```

Typical send patch:

```text
[42 3.14] → [bbb.zmq.send @endpoint tcp://*:5556 @type pub]
```

---

## 3. Packet and PacketView Model

### 3.1 Packet

A `Packet` represents one ZeroMQ message.

```cpp
struct Packet {
    PacketId id;
    std::vector<Frame> frames;
    std::string endpoint;
    double received_time;
};
```

A ZMQ multipart message maps directly to `frames`.

Example:

```text
frame[0] = "/sensor"
frame[1] = "/imu"
frame[2] = <binary imu payload>
```

### 3.2 PacketView

A `PacketView` is a lightweight view over a packet. It can represent the whole packet or a packet after one or more leading frames have been consumed.

```cpp
struct PacketView {
    ViewId id;
    std::shared_ptr<const Packet> packet;
    size_t start_frame;
};
```

A view-relative frame index resolves to:

```text
absolute_frame_index = start_frame + relative_frame_index
```

Patch cords still carry:

```text
packet <view-id>
```

The runtime decides whether the ID refers to an original packet or a derived view.

---

## 4. Routing Objects

## 4.1 `bbb.zmq.route`

`bbb.zmq.route` behaves like Max's `route`: it matches the current first frame and consumes it on successful match.

Example:

```text
[bbb.zmq.route /sensor]
|
[bbb.zmq.route /imu]
|
[bbb.zmq.parse imu]
```

Input view:

```text
view frame[0] = "/sensor"
view frame[1] = "/imu"
view frame[2] = <payload>
```

After `[bbb.zmq.route /sensor]`:

```text
view frame[0] = "/imu"
view frame[1] = <payload>
```

After `[bbb.zmq.route /imu]`:

```text
view frame[0] = <payload>
```

Then `[bbb.zmq.parse imu]` reads the current `frame[0]` by default.

### Matching behavior

`bbb.zmq.route` inspects the current view's `frame[0]`.

If `frame[0]` is a valid text key and matches one of the route arguments:

```text
matched outlet → packet <new-view-id>
```

The emitted view consumes one frame.

If it does not match:

```text
rightmost unmatched outlet → packet <same-view-id>
```

Unmatched packets are not consumed.

Invalid UTF-8, binary keys, missing frames, and single-frame raw binary payloads are treated as unmatched.

### Outlet behavior

For:

```text
[bbb.zmq.route /imu /camera]
```

Outlet mapping:

```text
outlet 0: matched /imu, consumed
outlet 1: matched /camera, consumed
outlet 2: unmatched, not consumed
```

---

## 4.2 `bbb.zmq.routepass`

`bbb.zmq.routepass` matches like `bbb.zmq.route`, but never consumes the frame.

Example:

```text
[bbb.zmq.routepass /imu]
```

Input view:

```text
view frame[0] = "/imu"
view frame[1] = <payload>
```

Output view:

```text
view frame[0] = "/imu"
view frame[1] = <payload>
```

This is useful when downstream objects also need the routing envelope.

---

## 4.3 Single-frame packets

A single-frame packet whose first frame is binary data is not routeable by default.

Recommended behavior:

```text
[bbb.zmq.route /imu /camera]
```

A single binary frame goes to the unmatched outlet unchanged.

Optional future attribute:

```text
@single unmatched | drop
```

---

## 4.4 Matching modes

v0.1 recommended minimum:

```text
@mode exact
```

Future:

```text
@mode prefix
```

---

## 5. Parser Object

## 5.1 `bbb.zmq.parse`

`bbb.zmq.parse` receives `packet <view-id>` and parses a selected frame from the current view using a compiled schema.

Constructor forms:

```text
[bbb.zmq.parse imu]
[bbb.zmq.parse @schema imu]
[bbb.zmq.parse imu @frame 0]
[bbb.zmq.parse @file schemas/imu.zmqdsl]
```

Attributes:

```text
@schema <symbol>
@frame <int>
@maxbytes <int>
@maxitems <int>
@maxatoms <int>
@maxstring <int>
```

---

## 5.2 Frame selection

By default:

```text
@frame 0
```

`@frame` is relative to the current packet view.

After consuming route frames, the payload is usually view-relative frame 0:

```text
[bbb.zmq.recv]
|
[bbb.zmq.route /sensor]
|
[bbb.zmq.route /imu]
|
[bbb.zmq.parse imu]
```

If using `bbb.zmq.routepass`, the routing key is preserved, so parsing may need a non-zero frame:

```text
[bbb.zmq.routepass /imu]
|
[bbb.zmq.parse imu @frame 1]
```

Frame selection is deliberately part of `bbb.zmq.parse`, not the DSL. The DSL describes the binary layout inside one frame.

---

## 5.3 Parser outlets

Recommended outlet layout:

```text
left outlet  : parsed Max messages
right outlet : diagnostics / errors
```

Example errors:

```text
error imu packet 1234 offset 0 code const_mismatch field magic expected 0xCAFE got 0xDEAD
error imu packet 1235 offset 12 code out_of_bounds field payload length 4096 remaining 128
error imu packet 1236 offset 8 code unterminated_string field name maxstring 4096
error imu packet 1237 code frame_missing frame 1
error imu packet 1238 code invalid_utf8 field device_name
error imu packet 1239 code too_many_atoms field values atoms 8192 max 1024
```

---

## 6. Schema Registry

## 6.1 `bbb.zmq.schema`

`bbb.zmq.schema` reads, stores, compiles, and registers parser schemas.

Attributes:

```text
@name <symbol>
@file <path>
@autocompile 0|1
```

Future:

```text
@watch 0|1
```

Messages:

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

Recommended outlets:

```text
left outlet  : status
right outlet : errors / diagnostics
```

Example success:

```text
compiled imu
schema imu fields 8 emits 4
```

Example compile error:

```text
error line 7 column 12 code expected_semicolon
error line 9 column 5 code unknown_type type "uint32"
```

---

## 6.2 Registry lookup

`bbb.zmq.schema` registers compiled schemas in a package-local runtime registry.

```cpp
class SchemaRegistry {
public:
    void register_schema(std::string name, std::shared_ptr<CompiledSchema>);
    std::shared_ptr<CompiledSchema> find(std::string name);
};
```

`[bbb.zmq.parse imu]` looks up `imu` in the registry.

If not found:

```text
error parse schema_not_found imu
```

---

## 6.3 Re-registration

Re-registering a schema name replaces the previous compiled schema.

Recommended implementation:

```cpp
struct SchemaEntry {
    uint64_t version;
    std::shared_ptr<CompiledSchema> schema;
};
```

`bbb.zmq.parse` may cache a schema pointer and refresh only when the registry version changes.

---

## 7. DSL Overview

The DSL describes the byte layout of a single frame. It does not describe which frame to read.

Example:

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

---

## 8. Directives

## 8.1 `endian`

```text
endian little;
endian big;
```

Applies to:

```text
u16 i16 u32 i32 u64 i64 f32 f64
```

---

## 8.2 `encoding`

```text
encoding utf8;
```

Used by `string` and bytes-to-string emit modifiers.

v0.1 default:

```text
utf8
```

Invalid UTF-8 produces a parse error.

Future:

```text
encoding ascii;
encoding bytes;
```

---

## 8.3 `onerror`

```text
onerror error;
onerror drop;
onerror pass;
```

Meaning:

```text
error : emit structured error on diagnostics outlet
drop  : emit nothing
pass  : pass original packet view to diagnostics/pass outlet
```

Recommended default:

```text
onerror error;
```

---

## 8.4 Limits

```text
maxbytes 1048576;
maxitems 4096;
maxatoms 1024;
maxstring 4096;
```

Meaning:

```text
maxbytes  : maximum size of bytes fields
maxitems  : maximum array element count
maxatoms  : maximum atom count emitted as a Max list
maxstring : maximum search length for null-terminated strings
```

These can also be overridden by `bbb.zmq.parse` attributes.

---

## 9. Primitive Types

v0.1 primitive types:

```text
u8   i8
u16  i16
u32  i32
u64  i64
f32  f64
```

Examples:

```text
u8 version;
u16 count;
u32 payload_len;
u64 timestamp;
f32 temperature;
f64 precise_time;
```

---

## 10. Fields

## 10.1 Scalar fields

```text
<type> <name>;
```

Example:

```text
u32 seq;
f32 temperature;
```

---

## 10.2 Constant validation

```text
<type> <name> = <literal>;
```

Example:

```text
u16 magic = 0xCAFE;
u8 version = 1;
```

If the parsed value does not match, parsing fails.

---

## 10.3 Fixed-length arrays

```text
<type> <name>[<integer>];
```

Example:

```text
f32 accel[3];
u16 values[16];
```

---

## 10.4 Variable-length arrays

A previously declared unsigned integer field can be used as the array length.

```text
u32 count;
f32 samples[count];
```

Constraints:

```text
- the length expression must refer to a previously declared unsigned integer field
- count * element_size must be checked for overflow
- count must not exceed maxitems
- list expansion must not exceed maxatoms
```

---

## 10.5 Fixed-length bytes

```text
bytes[<integer>] <name>;
```

Example:

```text
bytes[16] uuid;
```

---

## 10.6 Variable-length bytes

A previously declared unsigned integer field can be used as the byte length.

```text
u32 payload_len;
bytes[payload_len] payload;
```

Constraints:

```text
- the length expression must refer to a previously declared unsigned integer field
- length must not exceed the remaining frame size
- length must not exceed maxbytes
```

---

## 10.7 Remaining bytes

```text
bytes[*] payload;
```

Meaning:

```text
current offset through frame end
```

v0.1 restriction:

```text
bytes[*] must be the last field in the schema
```

---

## 10.8 Null-terminated string

```text
string <name>;
```

Example:

```text
string name;
```

Rules:

```text
- read from current offset until 0x00
- do not include 0x00 in the string value
- advance offset to the byte after 0x00
- fail if no 0x00 appears before frame end
- fail if search exceeds maxstring
- fail on invalid UTF-8 when encoding is utf8
```

---

## 10.9 Fixed-width string

```text
string[<integer>] <name>;
```

Example:

```text
string[32] device_name;
```

Rules:

```text
- read exactly N bytes
- advance offset by N bytes
- if 0x00 exists inside the field, string value ends before first 0x00
- bytes after first 0x00 are treated as padding
- if no 0x00 exists, all N bytes are treated as string content
- fail on invalid UTF-8 when encoding is utf8
```

This is intended to model C-style fixed-width `char name[N]` fields.

---

## 10.10 Disallowed string length expression

v0.1 does not allow:

```text
u32 len;
string[len] name;   // invalid
```

Rationale:

```text
string name;      // null-terminated string
string[32] name;  // fixed-width char array
```

Length-prefixed strings should be modeled as bytes plus an emit modifier:

```text
u32 name_len;
bytes[name_len] name_raw;
emit name name_raw@string;
```

---

## 10.11 Skip

```text
skip 4;
```

Skips a fixed number of bytes.

v0.1 only requires integer literals for `skip`.

Future:

```text
u16 pad_len;
skip pad_len;
```

---

## 11. Emit

`emit` defines Max message output.

```text
emit <selector> <expr...>;
```

Examples:

```text
emit timestamp timestamp;
emit accel accel[0] accel[1] accel[2];
emit gyro gyro[0] gyro[1] gyro[2];
```

Output:

```text
timestamp 123456789
accel 0.12 -0.03 9.81
gyro 0.01 0.02 0.00
```

### 11.1 Short form

```text
emit timestamp;
```

Equivalent to:

```text
emit timestamp timestamp;
```

### 11.2 Array expansion

```text
emit values values[*];
```

Expands an array into a Max atom list.

Constraint:

```text
expanded atom count must not exceed maxatoms
```

### 11.3 Quoted selector

```text
emit "/imu/accel" accel[0] accel[1] accel[2];
```

---

## 12. Emit Modifiers

## 12.1 `@handle`

Emits an internal handle for bytes or large arrays.

```text
emit payload payload@handle;
```

Example output:

```text
payload zmqbytes_1234_payload
```

or implementation-defined equivalent:

```text
payload packet 1234 field payload
```

`@handle` is the recommended default for large bytes fields.

---

## 12.2 `@list`

Emits bytes or arrays as a Max atom list.

```text
emit payload payload@list;
```

Example:

```text
payload 12 34 255 0 10
```

Constraint:

```text
atom count must not exceed maxatoms
```

---

## 12.3 `@string`

Decodes bytes using the schema encoding.

```text
u32 name_len;
bytes[name_len] name_raw;

emit name name_raw@string;
```

Rules:

```text
- decode all bytes
- invalid UTF-8 fails when encoding is utf8
- NUL byte should produce an error
```

---

## 12.4 `@cstring`

Decodes bytes as a C string.

```text
emit name name_raw@cstring;
```

Rules:

```text
- string content ends before the first 0x00
- if no 0x00 exists, use all bytes
- invalid UTF-8 fails when encoding is utf8
```

---

## 13. DSL Grammar Sketch

```text
program        := schema_decl*

schema_decl    := "schema" IDENT "{" schema_item* "}"

schema_item    := directive
                | field_decl
                | emit_decl

directive      := "endian" ("little" | "big") ";"
                | "encoding" IDENT ";"
                | "onerror" ("error" | "drop" | "pass") ";"
                | "maxbytes" INTEGER ";"
                | "maxitems" INTEGER ";"
                | "maxatoms" INTEGER ";"
                | "maxstring" INTEGER ";"

field_decl     := scalar_field
                | bytes_field
                | string_field
                | skip_field

scalar_field   := primitive IDENT array_suffix? const_suffix? ";"

primitive      := "u8" | "i8"
                | "u16" | "i16"
                | "u32" | "i32"
                | "u64" | "i64"
                | "f32" | "f64"

array_suffix   := "[" length_expr "]"

const_suffix   := "=" literal

bytes_field    := "bytes" "[" length_expr "]" IDENT ";"

string_field   := "string" IDENT ";"
                | "string" "[" INTEGER "]" IDENT ";"

skip_field     := "skip" INTEGER ";"

length_expr    := INTEGER | IDENT | "*"

emit_decl      := "emit" emit_selector emit_expr* ";"
                | "emit" IDENT ";"

emit_selector  := IDENT | STRING_LITERAL

emit_expr      := field_ref
                | literal

field_ref      := IDENT
                | IDENT "[" INTEGER "]"
                | IDENT "[" "*" "]"
                | IDENT "@" modifier

modifier       := "handle" | "list" | "string" | "cstring"

literal        := INTEGER
                | FLOAT
                | STRING_LITERAL
```

Additional constraints:

```text
- string[IDENT] is invalid
- bytes[*] must be the final field
- length_expr IDENT must refer to a previously declared unsigned integer field
- array length IDENT must refer to a previously declared unsigned integer field
- emit fields must already be declared
```

---

## 14. Example Schemas

### 14.1 IMU payload

ZMQ multipart:

```text
frame[0] = "/imu"
frame[1] = binary payload
```

Max patch using frame consumption:

```text
[bbb.zmq.recv]
|
[bbb.zmq.route /imu]
|
[bbb.zmq.parse imu]
```

DSL:

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

Output:

```text
timestamp 123456789
type 1
accel 0.12 -0.03 9.81
gyro 0.01 0.02 0.00
quat 1.0 0.0 0.0 0.0
```

### 14.2 Null-terminated name plus variable payload

Layout:

```text
string name, null terminated
u32 payload_len
payload bytes
```

DSL:

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

Output:

```text
name sensor_A
payload zmqbytes_1234_payload
```

### 14.3 Fixed-width C strings

C-like layout:

```c
char topic[16];
char device[32];
uint32_t payload_len;
uint8_t payload[payload_len];
```

DSL:

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

### 14.4 Length-prefixed string as bytes

Length-prefixed strings are modeled as `bytes[len] + @string`.

```text
schema lp_name {
  endian little;
  encoding utf8;

  u32 name_len;
  bytes[name_len] name_raw;

  emit name name_raw@string;
}
```

C-string interpretation:

```text
emit name name_raw@cstring;
```

### 14.5 Variable-length float samples

```text
schema samples {
  endian little;
  onerror error;
  maxitems 4096;
  maxatoms 1024;

  u32 count;
  f32 values[count];

  emit count count;
  emit values values[*];
}
```

Large samples should use handles or future matrix output:

```text
emit values values@handle;
```

---

## 15. File and Codebox Workflows

### 15.1 File-based schema

Recommended production workflow:

```text
[bbb.zmq.schema @file schemas/imu.zmqdsl]
[bbb.zmq.parse imu]
```

If the file contains:

```text
schema imu { ... }
```

then the schema registers as `imu`.

### 15.2 Explicit registration name

```text
[bbb.zmq.schema @name imu @file schemas/imu_v1.zmqdsl]
```

If the file declares:

```text
schema imu_v1 { ... }
```

it registers as `imu` while preserving `imu_v1` as metadata.

### 15.3 Multiple schemas per file

A file may contain:

```text
schema imu { ... }
schema gyro { ... }
schema status { ... }
```

`[bbb.zmq.schema @file sensors.zmqdsl]` registers all schemas.

If `@name` is provided with multiple schemas, this should produce an error:

```text
error code name_override_with_multiple_schemas
```

### 15.4 Codebox/textedit style

Patch-embedded prototyping workflow:

```text
[textedit]
|
[prepend set]
|
[bbb.zmq.schema @name imu @autocompile 1]

[bbb.zmq.parse imu]
```

Manual compile workflow:

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

The `bbb.zmq.schema` object stores a text buffer and compiles it on `compile` or, if enabled, on `set`.

---

## 16. Sender Object

## 16.1 `bbb.zmq.send`

`bbb.zmq.send` sends ZMQ messages. It runs a background thread with a queue so that Max's scheduler is never blocked.

v0.1 supports unidirectional socket types: `pub`, `push`, `pair`.

### Attributes

```text
@endpoint <symbol>   default: tcp://*:5556
@type <symbol>       default: pub        (pub, push, pair)
@bind <int>          default: 1          (1=bind, 0=connect)
@hwm <int>           default: 1000       (SNDHWM)
@endian <symbol>     default: big        (big, little)
```

### Messages

```text
start              create socket, start send thread
stop               stop send thread, cleanup
bang               alias for start
send <atoms>       encode atoms → single frame, send immediately
frame <atoms>      add type-aware frame to multipart buffer
frame_bytes <ints> add raw byte frame (each int = byte 0-255)
flush              send buffered frames as multipart, clear buffer
```

### Type-aware encoding

`send` and `frame` encode atoms by Max type:

| Max type | Encoding | Size |
|----------|----------|------|
| int/long | int64_t in configured byte order | 8 bytes |
| float | double (IEEE 754) in configured byte order | 8 bytes |
| symbol | raw UTF-8 bytes | variable |

Example:

```text
send sensor 42 3.14
```

Encodes to one frame:

```text
sensor       (6 bytes: s e n s o r)
42           (8 bytes: int64 BE 0x000000000000002A)
3.14         (8 bytes: double BE)
```

### Multipart sending

Use `frame` to buffer frames, then `flush` to send:

```text
[frame sensor]     \
[frame 42 3.14]     → same scheduler tick via trigger → bbb.zmq.send
[flush]            /
```

For explicit raw bytes:

```text
[frame_bytes 1 0 0 0 42]
```

### Thread model

Send operations are enqueued and processed by a background thread using `condition_variable`. This avoids blocking Max's scheduler and ensures zero CPU overhead when idle.

### Future

```text
- req/dealer socket types (bidirectional)
- bbb.zmq.encode standalone encoder
- reply output on separate outlet
```

---

## 17. v0.1 Non-goals

The following are intentionally deferred:

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
bbb.zmq.encode standalone binary encoder
req/dealer bidirectional sockets
```

These should be considered Phase 2 or later.

---

## 18. Recommended Initial Implementation

Implement first:

```text
bbb.zmq.recv
bbb.zmq.send
bbb.zmq.route
bbb.zmq.routepass
bbb.zmq.schema
bbb.zmq.parse

PacketStore
PacketView
SchemaRegistry
CompiledSchema
ParserVM or equivalent compiled parser
Max scheduler-safe dispatch bridge
```

## 19. DSL v0.1 implementation set

```text
schema
endian
encoding utf8
onerror error/drop/pass

u8/i8/u16/i16/u32/i32/u64/i64/f32/f64
fixed arrays
variable arrays
bytes[N]
bytes[len]
bytes[*]
string
string[N]
skip N

constant validation
emit
@handle
@list
@string
@cstring

maxbytes/maxitems/maxatoms/maxstring
```

