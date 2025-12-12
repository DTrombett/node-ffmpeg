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
	context.timeBase,
	context.timeBase.num,
	context.timeBase.den,
);
console.time();
context.timeBase = { num: 1, den: 30 };
console.timeEnd();
console.log(
	"Context after setting properties:",
	context.timeBase,
	context.timeBase.num,
	context.timeBase.den,
);
