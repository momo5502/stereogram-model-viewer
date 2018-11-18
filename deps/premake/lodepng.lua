lodepng = {
	source = "deps/lodepng"
}

function lodepng.import()
	links { "lodepng" }
	lodepng.includes()
end

function lodepng.includes()
	includedirs { lodepng.source }
	
	defines {

	}
end

function lodepng.project()
	project "lodepng"
		language "C++"

		zlib.import()
		lodepng.includes()
		files
		{
			path.join(lodepng.source, "lodepng.h"),
			path.join(lodepng.source, "lodepng.cpp"),
			--path.join(lodepng.source, "lodepng_util.h"),
			--path.join(lodepng.source, "lodepng_util.cpp"),
		}

		warnings "Off"

		kind "StaticLib"
end