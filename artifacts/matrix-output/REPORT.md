# Matrix output host acceptance — PASS

Checked 2026-11-06 JST. Base main: `1e5cf4b15f13b6587e99e36e4bdbbcd3a9b275c4`.

The prior PR 8 head `38db6215ae154274734f498705cfcc27277c89d6` emitted only `image` for the same normal 1-row and 2D inputs. The complete fix now emits a usable `jit_matrix` through the normal Min outlet. Values were read from that emitted matrix name, not inferred from HTTP or build success.

## Build and host

- macOS 26.4 (25E246), ARM64, Max 9.1.5 (3db35fa476d).
- Fresh checkout: `/Users/2bit/prog/bbb-max/bbb.zmq-output`; pinned submodules; out-of-source Release build succeeded for all seven externals. Each bundle contains arm64 and x86_64. libzmq emitted existing initializer warnings.
- Actual mapped parse binary was verified in each dedicated Max process using lsof. Paths and PIDs are in [results.json](results.json).
- C++ source SHA-256: `dbabe019bff21e7f1a4352a9e5091996d67234cc1a7c741d1585b5c5f6fe2dd3`.
- Built parse binary SHA-256: `eaa2f1c01f6b8266332998dd07228d766c0119cde48e39de795d682cebb6bce7`.
- Full local configure/build/run logs: `/Users/2bit/prog/bbb-max/artifacts/bbb-zmq-matrix-output/`.

## Real matrix observations

Each row below passed exact comparisons of dimensions, plane count, type, every data value, and allocation/stride bounds.

| Scenario | Dim | Planes / type | Byte strides | Data |
| --- | --- | --- | --- | --- |
| Single row | [5,1] | 1 / char | [1,16] | 11,22,33,44,55 |
| Two rows | [3,2] | 3 / char | [3,16] | 1..18 |
| Grow cached matrix | [5,3] | 3 / char | [3,16] | 1..45 |
| Shrink cached matrix | [2,1] | 3 / char | [3,16] | 21..26 |
| Recompile same schema to float32 / one plane | [3,2] | 1 / float32 | [4,16] | 1.25,-2.5,3.75,4.5,5.25,6 |
| Restore char after rejected type | [3,2] | 3 / char | [3,16] | 1..18 |

The single-row input remains `N x 1` with dimcount 2, matching the DSL's two required dimensions. No support/coverage of dimcount 1 is claimed. The observed 16-byte row stride is Jitter's value on this host; the code does not hard-code it.

## Errors and cleanup

- In each patch, invalid packet handle was rejected with `packet_not_found`.
- Truncated payloads produced `out of bounds`; zero width produced `matrix dims must be > 0`.
- A u16 matrix payload was rejected with `matrix_invalid_layout` and no matrix output. A subsequent valid char payload succeeded on the same parser and cache.
- Both Max processes remained responsive, stopped their receivers and closed the dedicated patches. Each event log ends in `patch_deleted`; ports 59481/59482 were released.
- The observer first read a separate known-good Jitter matrix to validate its capture path. The table above comes from actual parse output, separately recorded as `parsed_matrix_metadata` and `parsed_matrix_data`.

[results.json](results.json) contains the assertions; [events.json](events.json) contains the timestamped outlet messages and metadata/data. The reusable [host rig](../../tests/max/matrix-output/README.md) includes the generator, independent libzmq sender, runner, and reproduction commands. The sender does not use bbb.zmq.send, avoiding dependence on its matrix extraction.

## Scope and limits

The matrix constructor now receives `t_jit_matrix_info*`. Copying uses lock/getinfo/getdata with RAII restoration, checks native type/shape, source length, actual row stride and final allocation offset, and updates cached layout when it changes. Errors are reported and suppress `jit_matrix`; the existing scalar selector emission is retained.

Raw-copy matrix types are u8/i8, u32/i32, f32 and f64. Jitter has no matching 16/64-bit integer cells, so those inputs are explicitly rejected rather than misinterpreted or truncated. Endian conversion of matrix payloads is outside this fix; the float32 test uses native little-endian data. Char and float32 were exercised; other supported types were not individually tested.

Native allocation/getinfo failures were not injected, and lock restoration on such failures was reviewed in code rather than fault-tested. Windows runtime, Intel execution, DSP and Ableton Live were not tested. The existing Max process, user patches and installed packages were untouched. Dedicated Max application instances remain idle after their test patches close; none was force-quit.

## SDK basis

- Native constructor and getinfo/setinfo contract: [Cycling 74 Matrix Module](https://sdk.cdn.cycling74.com/max-sdk-8.2.0/group__matrixmod.html), also pinned SDK jit.thin.c and jit.demultiplex.c.
- Outlet pointer contract: pinned min-api c74_min_outlet.h; matrix notification now uses output.send.
- Host matrix observer: [JitterMatrix JS API](https://docs.cycling74.com/apiref/js/jittermatrix/).
