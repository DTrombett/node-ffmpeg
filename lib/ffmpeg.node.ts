// enum AVMediaType {
// 	AVMEDIA_TYPE_UNKNOWN = -1,
// 	AVMEDIA_TYPE_VIDEO,
// 	AVMEDIA_TYPE_AUDIO,
// 	AVMEDIA_TYPE_DATA,
// 	AVMEDIA_TYPE_SUBTITLE,
// 	AVMEDIA_TYPE_ATTACHMENT,
// 	AVMEDIA_TYPE_NB,
// }

// declare module "ffmpeg.node" {
// 	export type AVProfile = Readonly<{
// 		profile: number;
// 		name: string;
// 	}>;
// 	export type AVCodec = Readonly<{
// 		name: string;
// 		longName: string;
// 		type: AVMediaType;
// 		id: number;
// 		capabilities: number;
// 		maxLowres: number;
// 		wrapperName?: string;
// 		profiles?: AVProfile[];
// 	}>;
// 	export function versionInfo(): string;
// 	export function findEncoderByName(name: string): AVCodec | undefined;
// }
