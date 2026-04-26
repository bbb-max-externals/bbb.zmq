// send-text.js — Send plain text messages to Max via ZMQ pub/sub
// Max side: test-recv-parse.maxpat must be running with recv bound on port 5555
//
// These go to the "unmatched" outlet of route since there's no /text route key.
// Max will show them via peek (verbose 2) — check the debug print.
//
// Expected Max console output (from peek):
//   debug: peek view_id N packet_id N start_frame 0 frames 1
//   debug: frame 0 size <N> hello from node

const zmq = require("zeromq");

async function main() {
  const pub = new zmq.Publisher();
  await pub.connect("tcp://localhost:5555");
  console.log("[send-text] Connected to tcp://localhost:5555");

  await new Promise((r) => setTimeout(r, 1000));

  const messages = [
    "hello from node",
    "bbb.zmq test message",
    "multipart follows",
  ];

  for (let i = 0; i < messages.length; i++) {
    await pub.send([Buffer.from("plain-text"), Buffer.from(messages[i])]);
    console.log(`[send-text] #${i + 1} "${messages[i]}"`);
    if (i < messages.length - 1) await new Promise((r) => setTimeout(r, 200));
  }

  // Send a single-frame message (goes to unmatched, shown via peek)
  await new Promise((r) => setTimeout(r, 300));
  await pub.send(Buffer.from("single frame text"));
  console.log('[send-text] Single frame: "single frame text"');

  pub.close();
  console.log("[send-text] Done — check Max console (peek debug output)");
}

main().catch((e) => {
  console.error("[send-text] Error:", e.message);
  process.exit(1);
});
