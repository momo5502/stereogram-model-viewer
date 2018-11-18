tinyobjloader = {
	source = "deps/tinyobjloader"
}

function tinyobjloader.import()
	links { "tinyobjloader" }
	tinyobjloader.includes()
end

function tinyobjloader.includes()
	includedirs { tinyobjloader.source }
	
	defines
	{
		"TINYOBJLOADER_USE_DOUBLE"
	}
end

function tinyobjloader.project()
	project "tinyobjloader"
		language "C++"

		tinyobjloader.includes()
		files
		{
			path.join(tinyobjloader.source, "tiny_obj_loader.h"),
			path.join(tinyobjloader.source, "tiny_obj_loader.cc"),
		}
		defines
		{
			"_CRT_SECURE_NO_DEPRECATE",
		}

		warnings "Off"

		kind "StaticLib"
end