#!/usr/bin/env node
// run-all.js — Run all send tests sequentially
// Prerequisite: test-recv-parse.maxpat must be open in Max with schemas compiled.

const { execSync } = require("child_process");
const path = require("path");

const tests = [
  { name: "IMU binary parse", script: "send-imu.js" },
  { name: "Simple binary parse", script: "send-simple.js" },
  { name: "Text messages", script: "send-text.js" },
];

console.log("=== bbb.zmq Node.js Test Runner ===");
console.log("Make sure test-recv-parse.maxpat is open in Max!\n");

for (const test of tests) {
  console.log(`--- ${test.name} ---`);
  try {
    execSync(`node ${path.join(__dirname, test.script)}`, {
      stdio: "inherit",
      timeout: 15000,
    });
  } catch (e) {
    console.error(`FAILED: ${test.name}`);
    process.exit(1);
  }
  console.log("");
  execSync("sleep 1");
}

console.log("=== All tests sent ===");
console.log("Verify Max console output matches expected values.");
