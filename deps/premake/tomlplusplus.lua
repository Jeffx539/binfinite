tomlplusplus = {
	source = path.join(dependencies.basePath, "tomlplusplus"),
}

function tomlplusplus.import()
	links { "tomlplusplus" }
	tomlplusplus.includes()
end

function tomlplusplus.includes()
	includedirs {
		path.join(tomlplusplus.source, "include")
	}
end

function tomlplusplus.project()
	project "tomlplusplus"
		language "C"

		minhook.includes()

		files {
			path.join(minhook.source, "src/**.h"),
			path.join(minhook.source, "src/**.c"),
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, tomlplusplus)