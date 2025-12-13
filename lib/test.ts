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
let ret = ffmpeg.optSet(context.privData, "preset", "veryslow", 0);
ret ||= ffmpeg.optSet(context.privData, "qp", "0", 0);
ret ||= ffmpeg.optSet(context.privData, "weightp", "2", 0);
if (!ret) console.log("Context after setting properties:", context);
else
	console.error(
		"Error setting context properties:",
		String.fromCharCode(
			-ret & 0xff,
			(-ret >> 8) & 0xff,
			(-ret >> 16) & 0xff,
			(-ret >> 24) & 0xff,
		),
	);
// 1765645200
