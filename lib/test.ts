import ffmpeg from "ffmpeg.node";

console.log("ffmpeg version:", ffmpeg.versionInfo());
const codec = ffmpeg.findEncoderByName("libx264");
console.log("Codec:", codec);
const context = ffmpeg.allocContext3(codec);
console.log("Context:", context);
const packet = ffmpeg.packetAlloc();
console.log("Packet:", packet);
context.width = 1280;
context.height = 720;
context.timeBase = { num: 1, den: 24 };
context.framerate = { num: 24, den: 1 };
context.gopSize = 240;
context.maxBFrames = 0;
context.pixFmt = 0;
context.profile = 144;
console.log("Context after setting properties:", context);
