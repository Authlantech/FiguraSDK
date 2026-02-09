cmake_minimum_required (VERSION 3.10)

add_library (
		FiguraSDK STATIC
		../source/camera.cpp
		../source/mesh.cpp
		../source/model.cpp
		../source/shader.cpp
		../source/glad.c
		../source/texture.cpp
		../source/GraphicsEngine.cpp
)

target_include_directories(
		FiguraSDK
		PUBLIC ../include
)

