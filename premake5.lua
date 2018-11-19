require "deps/premake/stb"
require "deps/premake/gsl"
require "deps/premake/glm"
require "deps/premake/glew"
require "deps/premake/glfw"
require "deps/premake/tinyobjloader"

workspace "model-viewer"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }

	buildoptions "/std:c++latest"

	configuration "windows"
		defines { "_WINDOWS", "WIN32" }
		flags { "StaticRuntime" }

		if symbols ~= nil then
			symbols "On"
		else
			flags { "Symbols" }
		end

	configuration "Release"
		defines { "NDEBUG" }
		flags { "MultiProcessorCompile", "LinkTimeOptimization" }
		optimize "Full"

	configuration "Debug"
		defines { "DEBUG", "_DEBUG" }
		flags { "MultiProcessorCompile" }
		optimize "Debug"

	project "model-viewer"
		kind "WindowedApp"
		language "C++"
		files {
			"./src/**.rc",
			"./src/**.hpp",
			"./src/**.cpp",
		}
		includedirs {
			"./src"
		}
		resincludedirs {
			"$(ProjectDir)src" -- fix for VS IDE
		}

		-- Pre-compiled header
		pchheader "std_include.hpp" -- must be exactly same as used in #include directives
		pchsource "src/std_include.cpp" -- real path
		buildoptions { "/Zm100 -Zm100" }

		vpaths {
			["Docs/*"] = { "**.txt","**.md" },
		}

		-- Specific configurations
		flags { "UndefinedIdentifiers", "ExtraWarnings" }

		configuration "Release*"
			flags { "FatalCompileWarnings" }
		configuration {}
		
		stb.includes()
		gsl.includes()
		glm.includes()
		
		glew.import()
		glfw.import()
		tinyobjloader.import()

	group "Dependencies"
		glew.project()
		glfw.project()
		tinyobjloader.project()