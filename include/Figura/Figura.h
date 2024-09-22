#pragma once 

#include <Figura/camera.h>
#include <Figura/indexbuffer.h>
#include <Figura/light.h>
#include <Figura/mesh.h>
#include <Figura/model.h>
#include <Figura/shader.h>
#include <Figura/texture.h>
#include <Figura/vertexbuffer.h>


namespace fgr
{

	static Shader default_shader, normal_shader, mesh_shader;

	void InitFigura()
	{
		//Begin Light Buffers 
		fgr::DirectionalLight::begin_directional_lights(); 
		fgr::PointLight::begin_point_lights(); 
		fgr::SpotLight::begin_spot_light();

		// Prepare hardcoded shaders : 
		default_shader.create(); 
		normal_shader.create(); 
		mesh_shader.create(); 

		default_shader.load_from_buffer(default_vs, default_fs); 
		normal_shader.load_from_buffer(normal_vs, normal_fs, normal_gs); 
		mesh_shader.load_from_buffer(mesh_vs, mesh_fs, mesh_gs);

		//Enable Depth Testing and Face Culling : 
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}; 

	void ClearColor(float r, float g, float b)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(r, g, b, 1.f);
	}

}