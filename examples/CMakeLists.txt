cmake_minimum_required (VERSION 3.8)

include_directories(
	"../include",
	"imgui"	
)

link_directories(
	"../lib"
)

link_libraries(
	"opengl32.lib"
	"FiguraSDK.lib"
	"assimp-vc143-mt.lib"
	"glfw3.lib"
)

add_executable(
	"example"
	"example.cpp"

	"imgui/imgui.cpp"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_tables.cpp"
	"imgui/imgui_widgets.cpp"
	"imgui/backends/imgui_impl_glfw.cpp"
	"imgui/backends/imgui_impl_opengl3.cpp"
)