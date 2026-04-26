// send-imu.js — Send binary IMU data to Max via ZMQ pub/sub
// Max side: test-recv-parse.maxpat must be running with recv bound on port 5555
//
// Payload matches schemas/imu.zmqdsl:
//   little-endian: u16 magic(0xCAFE) + u8 version + u8 type + u64 timestamp + f32 accel[3] + f32 gyro[3] = 36 bytes
//
// Expected Max console output:
//   imu: timestamp <ms> version 1 type 2 accel 0.12 -0.03 9.81 gyro 0.01 0.02 0

const zmq = require("zeromq");

function buildImuPacket(version, type, accel, gyro) {
  const buf = Buffer.alloc(36);
  buf.writeUInt16LE(0xcafe, 0);
  buf.writeUInt8(version, 2);
  buf.writeUInt8(type, 3);
  buf.writeBigUInt64LE(BigInt(Date.now()), 4);
  buf.writeFloatLE(accel[0], 12);
  buf.writeFloatLE(accel[1], 16);
  buf.writeFloatLE(accel[2], 20);
  buf.writeFloatLE(gyro[0], 24);
  buf.writeFloatLE(gyro[1], 28);
  buf.writeFloatLE(gyro[2], 32);
  return buf;
}

async function main() {
  const pub = new zmq.Publisher();
  await pub.connect("tcp://localhost:5555");
  console.log("[send-imu] Connected to tcp://localhost:5555");

  // Wait for subscription to establish
  await new Promise((r) => setTimeout(r, 1000));

  const packets = [
    { v: 1, t: 2, a: [0.12, -0.03, 9.81], g: [0.01, 0.02, 0.0] },
    { v: 1, t: 3, a: [1.5, 2.5, 3.5], g: [-0.1, -0.2, -0.3] },
    { v: 2, t: 1, a: [0.0, 0.0, 0.0], g: [0.0, 0.0, 0.0] },
  ];

  for (let i = 0; i < packets.length; i++) {
    const p = packets[i];
    const payload = buildImuPacket(p.v, p.t, p.a, p.g);
    await pub.send(["/imu", payload]);
    console.log(
      `[send-imu] #${i + 1} version=${p.v} type=${p.t} accel=[${p.a}] gyro=[${p.g}]`
    );
    if (i < packets.length - 1) await new Promise((r) => setTimeout(r, 200));
  }

  pub.close();
  console.log("[send-imu] Done — check Max console for parsed output");
}

main().catch((e) => {
  console.error("[send-imu] Error:", e.message);
  process.exit(1);
});
