#pragma once 

#include <Figura/camera.h>
#include <Figura/indexbuffer.h>
#include <Figura/light.h>
#include <Figura/mesh.h>
#include <Figura/model.h>
#include <Figura/shader.h>
#include <Figura/texture.h>
#include <Figura/vertexbuffer.h>
#include <GLFW/glfw3.h>


namespace fgr
{

	class RenderingWindow
	{
	private :

		fgr::Shader	current_shader;
		fgr::Shader	model_shader; 
		fgr::Shader	normal_shader; 
		fgr::Shader	mesh_shader; 

		GLFWwindow* window = nullptr;

	public : 
		
		friend class fgr::Model;
		friend class fgr::Camera;

		void init_window(int width, int height,const char* title);
		void clear(float red,float green,float blue,float alpha);
		void update();
		bool is_open();

		GLFWwindow* _get_window();
	};

	extern RenderingWindow default_window;
}