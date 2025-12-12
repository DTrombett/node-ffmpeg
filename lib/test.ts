import ffmpeg from "ffmpeg.node";

console.log("ffmpeg version:", ffmpeg.versionInfo());
console.time();
const encoder = ffmpeg.findEncoderByName("libvpx-vp9");
console.timeEnd();
console.log("vp9 codec:", encoder);
console.time();
const context = ffmpeg.allocContext3(encoder);
console.timeEnd();
console.log(
	"Context before setting properties:",
	context.width,
	context.height,
);
console.time();
context.width = 1920;
context.height = 1080;
console.timeEnd();
console.log("Context after setting properties:", typeof context.privData);
