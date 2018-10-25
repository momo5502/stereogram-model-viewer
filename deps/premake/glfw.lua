glfw = {
	source = "deps/glfw"
}

function glfw.import()
	links { "glfw" }
	glfw.includes()
end

function glfw.includes()
	includedirs { path.join(glfw.source, "include") }
	
	defines {
		"_GLFW_WIN32"
	}
end

function glfw.project()
	project "glfw"
		language "C"

		glfw.includes()
		files
		{
			path.join(glfw.source, "src/**.c")
		}
		
		removefiles
		{
			path.join(glfw.source, "src/cocoa_*.c"),
			path.join(glfw.source, "src/wl_*.c"),
			path.join(glfw.source, "src/x11_*.c"),
			path.join(glfw.source, "src/posix_*.c"),
			path.join(glfw.source, "src/null_*.c"),
			path.join(glfw.source, "src/linux_*.c"),
			path.join(glfw.source, "src/glx_*.c"),
		}

		removelinks "*"
		warnings "Off"
		kind "StaticLib"
end