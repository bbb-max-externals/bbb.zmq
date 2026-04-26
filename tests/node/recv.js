// recv.js — Receive messages from Max bbb.zmq.send via ZMQ pub/sub
// Max side: test-send.maxpat must be running with send bound on port 5556
//
// Usage: node recv.js
// Then click messages in the test-send.maxpat to send data.

const zmq = require("zeromq");

async function main() {
  const sub = new zmq.Subscriber();
  await sub.connect("tcp://localhost:5556");
  sub.subscribe();
  console.log("[recv] Listening on tcp://localhost:5556 — waiting for Max to send...");
  console.log("[recv] (send messages from test-send.maxpat, or Ctrl+C to stop)\n");

  let count = 0;
  const timeout = setTimeout(() => {
    console.log("[recv] Timeout (10s) — no messages received");
    process.exit(1);
  }, 10000);

  for await (const frames of sub) {
    clearTimeout(timeout);
    count++;
    console.log(`[recv] Message #${count} — ${frames.length} frame(s):`);
    for (let i = 0; i < frames.length; i++) {
      const f = frames[i];
      const isText = !f.some((b) => b > 0x7e || (b < 0x20 && b !== 0x09 && b !== 0x0a && b !== 0x0d));
      if (isText) {
        console.log(`  frame[${i}] text: "${f.toString()}"`);
      } else {
        const hex = f.toString("hex").match(/.{1,2}/g).join(" ");
        console.log(`  frame[${i}] ${f.length} bytes: ${hex.substring(0, 60)}${f.length > 30 ? " ..." : ""}`);
      }
    }
    console.log("");
  }
}

main().catch((e) => {
  console.error("[recv] Error:", e.message);
  process.exit(1);
});
