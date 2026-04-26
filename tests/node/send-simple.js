// send-simple.js — Send binary "simple" sensor data to Max via ZMQ pub/sub
// Max side: test-recv-parse.maxpat must be running with recv bound on port 5555
//
// Payload matches schemas/simple.zmqdsl:
//   big-endian: u8 id + i32 value + f32 temp = 9 bytes
//
// Expected Max console output:
//   simple: id 1 value 42 temp 23.5

const zmq = require("zeromq");

function buildSimplePacket(id, value, temp) {
  const buf = Buffer.alloc(9);
  buf.writeUInt8(id, 0);
  buf.writeInt32BE(value, 1);
  buf.writeFloatBE(temp, 5);
  return buf;
}

async function main() {
  const pub = new zmq.Publisher();
  await pub.connect("tcp://localhost:5555");
  console.log("[send-simple] Connected to tcp://localhost:5555");

  await new Promise((r) => setTimeout(r, 1000));

  const packets = [
    { id: 1, value: 42, temp: 23.5 },
    { id: 2, value: -100, temp: -10.25 },
    { id: 3, value: 0, temp: 0.0 },
  ];

  for (let i = 0; i < packets.length; i++) {
    const p = packets[i];
    const payload = buildSimplePacket(p.id, p.value, p.temp);
    await pub.send(["/sensor", payload]);
    console.log(
      `[send-simple] #${i + 1} id=${p.id} value=${p.value} temp=${p.temp}`
    );
    if (i < packets.length - 1) await new Promise((r) => setTimeout(r, 200));
  }

  pub.close();
  console.log("[send-simple] Done — check Max console for parsed output");
}

main().catch((e) => {
  console.error("[send-simple] Error:", e.message);
  process.exit(1);
});
