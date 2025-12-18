import ffmpeg from "ffmpeg.node";
import { createWriteStream, type WriteStream } from "node:fs";

let ret = 0;
const checkRet = () => {
	if (ret)
		throw new Error("Error setting context properties:", {
			cause: String.fromCharCode(
				-ret & 0xff,
				(-ret >> 8) & 0xff,
				(-ret >> 16) & 0xff,
				(-ret >> 24) & 0xff,
			),
		});
};
const encode = async (
	context: any,
	frame: any,
	packet: any,
	file: WriteStream,
) => {
	ret = ffmpeg.sendFrame(context, frame);
	checkRet();
	while (ret >= 0) {
		ret = ffmpeg.receivePacket(context, packet);
		if (ret == -11 || ret == -541478725) return;
		checkRet();
		const { promise, resolve } = Promise.withResolvers<void>();
		file.write(new Uint8Array(packet.data), () => {
			ffmpeg.packetUnref(packet);
			resolve();
		});
		await promise;
	}
};

(async () => {
	console.log("ffmpeg version:", ffmpeg.versionInfo());
	console.time("encode time");
	const codec = ffmpeg.findEncoderByName("libx264");
	// console.log("Codec:", codec);
	const context = ffmpeg.allocContext3(codec);
	// console.log("Context:", context);
	console.log(codec === context.codec);
	const packet = ffmpeg.packetAlloc();
	// console.log("Packet:", packet);
	context.width = 352;
	context.height = 288;
	context.timeBase = { num: 1, den: 24 };
	context.framerate = { num: 24, den: 1 };
	context.gopSize = 240;
	context.maxBFrames = 0;
	context.pixFmt = 0;
	context.profile = 144;
	ret = ffmpeg.optSet(context.privData, "preset", "ultrafast");
	ret ||= ffmpeg.optSet(context.privData, "qp", "0");
	ret ||= ffmpeg.optSet(context.privData, "x264-params", "scenecut=0");
	ret ||= ffmpeg.optSet(context.privData, "weightp", "2");
	checkRet();
	ret = ffmpeg.open2(context, codec);
	checkRet();
	const file = createWriteStream("tmp/output.mp4");
	const frame = ffmpeg.frameAlloc();
	// console.log("Frame:", frame);
	frame.format = context.pixFmt;
	frame.width = context.width;
	frame.height = context.height;
	ret = ffmpeg.frameGetBuffer(frame);
	checkRet();
	for (let i = 0; i < 24; i++) {
		ret = ffmpeg.frameMakeWritable(frame);
		checkRet();
		const [yb, ub, vb] = (frame.data as ArrayBuffer[]).map(
			(buf) => new Uint8Array(buf),
		);
		const [yl, ul, vl] = frame.linesize;
		const height = context.height;
		const width = context.width;

		for (let y = 0; y < height; y++)
			for (let x = 0; x < width; x++) yb[y * yl + x] = (x + y + i * 3) % 256;
		for (let y = 0; y < height / 2; y++)
			for (let x = 0; x < width / 2; x++) {
				ub[y * ul + x] = (128 + y + i * 2) % 256;
				vb[y * vl + x] = (64 + x + i * 5) % 256;
			}
		frame.pts = i;
		await encode(context, frame, packet, file);
	}
	await encode(context, undefined, packet, file);
	file.end();
	console.timeEnd("encode time");
})();
