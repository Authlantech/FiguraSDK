cmake_minimum_required (VERSION 3.8)


add_library ( 
	"FiguraSDK" STATIC 
	"../source/camera.cpp"
	"../source/indexbuffer.cpp"
	"../source/mesh.cpp"
	"../source/model.cpp"
	"../source/shader.cpp"
	"../source/vertexbuffer.cpp"
	"../source/glad.c"
	"../source/texture.cpp"
	"../source/light.cpp"
    )

target_include_directories("FiguraSDK" 
PUBLIC "../include"
)