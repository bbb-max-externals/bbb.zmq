# Matrix output host regression

This checks the real `bbb.zmq.recv -> bbb.zmq.parse` path with schemas registered by `bbb.zmq.schema`. It requires macOS, Max 9, Python 3, and a build of this checkout. No Python ZMQ package is needed: the sender links the repository's pinned libzmq.

The test detects the issue 7 failure (only a selector, no `jit_matrix`), checks matrix values and padded row strides, and exercises cached shape/type changes and rejection/recovery. It launches separate Max instances, creates only dedicated patches, binds loopback ports 59481/59482, then stops and closes those patches. The application instances remain idle; it never quits an existing Max instance.

From the repository root:

```sh
qa_dir=$(mktemp -d /tmp/bbb-zmq-matrix.XXXXXX)
cmake -S . -B "$qa_dir/build" -DCMAKE_BUILD_TYPE=Release
cmake --build "$qa_dir/build" -j 6
clang++ -std=c++17 -I extern/cppzmq -I extern/libzmq/include \
  tests/max/matrix-output/send.cpp "$qa_dir/build/extern/libzmq/lib/libzmq.a" \
  -o "$qa_dir/send"
python3 tests/max/matrix-output/generate.py "$qa_dir/host"
python3 tests/max/matrix-output/run.py "$qa_dir/host" --sender "$qa_dir/send"
```

The runner verifies the actual mapped `bbb.zmq.parse` binary with `lsof`, reads every cell through the received matrix name, and checks metadata/size/stride. It validates the observer against a known-good Jitter matrix first. It sends each packet once; a timeout is a failure, never a retransmission. Use a fresh directory for every run so old logs cannot pass a new run.

Results are in `host/results.json` and `host/events.json`; each patch also writes its own events.json as messages arrive. Native allocation/getinfo failures are not injected. `N x 1` is the single-row representation because the current matrix DSL always supplies two dimensions; this does not test `dimcount == 1`. The float32 payload uses native little-endian bytes, matching the test host; this is not endian-conversion coverage.

The repository's attached [report](../../../artifacts/matrix-output/REPORT.md) describes the recorded run and its limits. Building may regenerate package-info.json; do not include that generated change in the patch.
