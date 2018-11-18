zlib = {
	source = "deps/zlib"
}

function zlib.import()
	links { "zlib" }
	zlib.includes()
end

function zlib.includes()
	includedirs { zlib.source }
	
	defines {
		"ZLIB_CONST",
		"ssize_t=int"
	}
end

function zlib.project()
	project "zlib"
		language "C"

		zlib.includes()
		files
		{
			path.join(zlib.source, "*.h"),
			path.join(zlib.source, "*.c"),
		}
		defines
		{
			"ZLIB_DLL",
			"_CRT_SECURE_NO_DEPRECATE",
		}

		warnings "Off"

		kind "StaticLib"
end