cmake_minimum_required (VERSION 3.8)

include_directories(
	"../include"
	"include"
)

link_directories(
	"../lib"
)

link_libraries(
	"opengl32.lib"
	"assimp-vc143-mt.lib"
	"glfw3.lib"
)

add_executable(
		"example_glfw"

		"example_glfw.cpp"

		"../source/camera.cpp"
		"../source/indexbuffer.cpp"
		"../source/mesh.cpp"
		"../source/model.cpp"
		"../source/shader.cpp"
		"../source/vertexbuffer.cpp"
		"../source/glad.c"
		"../source/texture.cpp"
		"../source/light.cpp"
		"../source/Figura.cpp"
)