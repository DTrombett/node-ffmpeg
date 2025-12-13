import { build } from "esbuild";

await build({
	entryPoints: ["lib/index.ts", "lib/test.ts"],
	alias: {
		"ffmpeg.node": "./ffmpeg.node",
	},
	bundle: true,
	charset: "utf8",
	external: ["*.node"],
	keepNames: true,
	minify: true,
	outdir: "dist",
	packages: "external",
	platform: "node",
	sourcemap: true,
	target: "node24",
	treeShaking: true,
});
