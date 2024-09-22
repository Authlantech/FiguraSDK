cmake_minimum_required (VERSION 3.8)

include_directories(
	"../include"
	"deps/GLFW/include"
)

link_directories(
	"../lib"
	"deps/GLFW/lib-vc2022"
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
)