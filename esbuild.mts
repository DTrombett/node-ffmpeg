import { build } from "esbuild";
import { env } from "node:process";

await build({
	entryPoints: ["lib/index.ts", "lib/test.ts"],
	alias: {
		"ffmpeg.node": `../build/${
			env.NODE_ENV === "development" ? "Debug" : "Release"
		}/ffmpeg.node`,
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
