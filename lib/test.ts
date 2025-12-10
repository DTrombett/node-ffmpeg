import ffmpeg from "ffmpeg.node";

console.log("ffmpeg version:", ffmpeg.versionInfo());
console.time();
const encoder = ffmpeg.findEncoderByName("libvpx-vp9");
console.timeEnd();
console.log("vp9 codec:", encoder);
