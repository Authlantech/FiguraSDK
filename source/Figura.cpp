#include <Figura/Figura.h>

namespace fgr
{
	RenderingWindow default_window;


	void RenderingWindow::init_window(int width, int height, const char* title)
	{
		// Init GLFW
		int res = glfwInit();
		if (res == GLFW_FALSE)
		{
			printf("glfw could not be initilaized!\n");
			exit(-1);
		}

		// Create window
		window = glfwCreateWindow(width,height,title, 0, 0);
		glfwMakeContextCurrent(window);

		// Init glad 
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		glViewport(0, 0,width,height);

		//Begin Light Buffers 
		fgr::DirectionalLight::begin_directional_lights();
		fgr::PointLight::begin_point_lights();
		fgr::SpotLight::begin_spot_light();

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

	}

	void RenderingWindow::clear(float red,float green,float blue,float alpha)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(red,green,blue,alpha);
	}

	void RenderingWindow::update()
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	bool RenderingWindow::is_open()
	{
		return !glfwWindowShouldClose(window);
	}

	GLFWwindow* RenderingWindow::_get_window()
	{
		return window;
	}

}