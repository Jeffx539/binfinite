imgui = {
	source = path.join(dependencies.basePath, "imgui"),
}

function imgui.import()
	links { "imgui" }
	imgui.includes()
end

function imgui.includes()
	includedirs {
		path.join(imgui.source),
		path.join(imgui.source,"backends")
	}
end

function imgui.project()
	project "imgui"
		language "C++"

		

        includedirs {
			path.join(imgui.source),
			path.join(imgui.source,"backends")
        }

		
		files
		{
			path.join(imgui.source,"*.cpp"),
			path.join(imgui.source,"*.h"),
			path.join(imgui.source,"backends/imgui_impl_dx12.h"),
			path.join(imgui.source,"backends/imgui_impl_dx12.cpp"),
			path.join(imgui.source,"backends/imgui_impl_win32.h"),
			path.join(imgui.source,"backends/imgui_impl_win32.cpp")
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, imgui)