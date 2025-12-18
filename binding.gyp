{
	"targets": [
		{
			"target_name": "ffmpeg",
			"sources": [
				"src/main.c",
				"src/map.c",
				"src/utils.c"
			],
			"include_dirs": [
				"deps/ffmpeg/include"
			],
			"libraries": [
				"<(module_root_dir)/deps/ffmpeg/lib/avcodec.lib",
				"<(module_root_dir)/deps/ffmpeg/lib/avdevice.lib",
				"<(module_root_dir)/deps/ffmpeg/lib/avfilter.lib",
				"<(module_root_dir)/deps/ffmpeg/lib/avformat.lib",
				"<(module_root_dir)/deps/ffmpeg/lib/avutil.lib",
				"<(module_root_dir)/deps/ffmpeg/lib/swresample.lib",
				"<(module_root_dir)/deps/ffmpeg/lib/swscale.lib"
			]
		},
		{
			"target_name": "libavutil",
			"sources": [
				"src/libavutil.c",
				"src/map.c",
				"src/utils.c"
			],
			"include_dirs": [
				"deps/ffmpeg/include"
			],
			"libraries": [
				"<(module_root_dir)/deps/ffmpeg/lib/avutil.lib"
			]
		}
	]
}
