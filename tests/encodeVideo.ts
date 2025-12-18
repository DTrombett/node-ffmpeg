// @ts-expect-error: types not added yet
import ffmpeg from "ffmpeg.node";
import { open, type FileHandle } from "node:fs/promises";

let ret: number;
const encode = async (
	encCtx: any,
	frame: any,
	pkt: any,
	outFile: FileHandle,
) => {
	if (frame) console.log(`Send frame ${frame.pts}`);
	ret = ffmpeg.avcodec_send_frame(encCtx, frame);
	if (ret < 0) throw new Error("Error sending a frame for encoding");
	while (ret >= 0) {
		ret = ffmpeg.avcodec_receive_packet(encCtx, pkt);
		if (ret == -11 || ret == -541478725) return;
		else if (ret < 0) throw new Error("Error during encoding");
		console.log(`Write packet pts ${pkt.pts} (size=${pkt.size})`);
		await outFile.write(pkt.data);
		ffmpeg.av_packet_unref(pkt);
	}
};

const main = async (argc: number, argv: string[]) => {
	if (argc <= 3)
		throw new Error(`Usage: ${argv[0]} ${argv[1]} <output file> <codec name>`);
	const [, , filename, codecName] = argv;
	const codec = ffmpeg.avcodec_find_encoder_by_name(codecName);
	if (!codec) throw new Error(`Codec '${codecName}' not found`);
	const c = ffmpeg.avcodec_alloc_context3(codec);
	if (!c) throw new Error("Could not allocate video codec context");
	const pkt = ffmpeg.av_packet_alloc();
	if (!pkt) throw new Error("Could not allocate video packet");
	c.bitRate = 400000;
	c.width = 352;
	c.height = 288;
	c.timeBase = { num: 1, den: 25 };
	c.framerate = { num: 25, den: 1 };
	c.gopSize = 10;
	c.maxBFrames = 1;
	c.pixFmt = 0;
	if (codec.id === 27) ffmpeg.av_opt_set(c.privData, "preset", "slow", 0);
	ret = ffmpeg.avcodec_open2(c, codec, null);
	if (ret < 0) throw new Error(`Could not open codec: ${ffmpeg.err2str(ret)}`);
	const f = await open(filename!, "w");
	const frame = ffmpeg.av_frame_alloc();
	if (!frame) throw new Error("Could not allocate video frame");
	frame.format = c.pixFmt;
	frame.width = c.width;
	frame.height = c.height;
	ret = ffmpeg.av_frame_get_buffer(frame, 0);
	if (ret < 0) throw new Error("Could not allocate the video frame data");
	for (let i = 0; i < 25; i++) {
		ret = ffmpeg.av_frame_make_writable(frame);
		if (ret < 0) throw new Error("Frame not writable");
		const [yb, ub, vb] = frame.data;
		const [yl, ul, vl] = frame.linesize;
		const height = c.height;
		const width = c.width;

		for (let y = 0; y < height; y++)
			for (let x = 0; x < width; x++) yb![y * yl + x] = x + y + i * 3;
		for (let y = 0; y < height / 2; y++)
			for (let x = 0; x < width / 2; x++) {
				ub![y * ul + x] = 128 + y + i * 2;
				vb![y * vl + x] = 64 + x + i * 5;
			}
		frame.pts = i;
		await encode(c, frame, pkt, f);
	}
	await encode(c, null, pkt, f);
	if (codec.id === 1 || codec.id === 2)
		f.write(new Uint8Array([0, 0, 1, 0xb7]));
	f.close();
	ffmpeg.avcodec_free_context(c);
	ffmpeg.av_frame_free(frame);
	ffmpeg.av_packet_free(pkt);
};

main(process.argv.length, process.argv);
