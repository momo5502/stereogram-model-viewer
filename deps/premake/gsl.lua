gsl = {
	source = "deps/gsl"
}

function gsl.includes()
	includedirs { path.join(gsl.source, "include") }
end
