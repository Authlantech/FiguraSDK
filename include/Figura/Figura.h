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

	class RenderingWindow
	{
	private :

		fgr::Shader	current_shader;
		fgr::Shader	model_shader; 
		fgr::Shader	normal_shader; 
		fgr::Shader	mesh_shader; 

	public : 
		
		friend class fgr::Model;
		friend class fgr::Camera; 


		void init_window()
		{
			// Start and load shaders
			model_shader.create();
			normal_shader.create();
			mesh_shader.create();

			model_shader.load_from_buffer(default_vs, default_fs);
			normal_shader.load_from_buffer(normal_vs, normal_fs, normal_gs);
			mesh_shader.load_from_buffer(mesh_vs, mesh_fs, mesh_gs);

			current_shader = model_shader;
			current_shader.use();

			//Enable Depth Testing and Face Culling : 
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			//Begin Light Buffers 
			fgr::DirectionalLight::begin_directional_lights();
			fgr::PointLight::begin_point_lights();
			fgr::SpotLight::begin_spot_light();
			
		}

		void clear()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.3, 0.3, 0.3, 0);
		}
	};

	static RenderingWindow default_window;
}