#include <Figura/Figura.h>

namespace fgr
{
	GraphicsEngine graphic_engine;


	void GraphicsEngine::init_engine(int window_width, int window_height, const char* window_title, int opengl_version_major, int opengl_version_minor, int opengl_profile)
	{
		// Init GLFW
		int res = glfwInit();
		if (res == GLFW_FALSE)
		{
			printf("glfw could not be initilaized!\n");
			exit(-1);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_version_major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_version_minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, opengl_profile);

		// Create window
		window = glfwCreateWindow(window_width, window_height, window_title, 0, 0);
		glfwMakeContextCurrent(window);

		// Init glad 
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		std::cout << "" << glGetString(GL_VERSION) << "\n\n";

		glViewport(0, 0, window_width, window_height);

		//Begin Light Buffers 
		fgr::DirectionalLight::begin_directional_lights();
		fgr::PointLight::begin_point_lights();
		fgr::SpotLight::begin_spot_light();

		// Start and load shaders

		create_shader("pbr shader", default_vs, pbrShader); 
		create_shader("phong shader", default_vs, default_fs);
		create_shader("normal shader", normal_vs, normal_fs,normal_gs); 
		create_shader("mesh shader", mesh_vs, mesh_fs, mesh_gs);
		
		use_shader("phong shader");

		//Enable Depth Testing and Face Culling : 
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::thread model_loading_thread(fgr::Model::model_loading_thread);
		model_loading_thread.detach();

	}

	void GraphicsEngine::clear_window(float red,float green,float blue,float alpha)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(red,green,blue,alpha);
	}

	void GraphicsEngine::update_window()
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	bool GraphicsEngine::window_is_open()
	{
		return !glfwWindowShouldClose(window);
	}

	GLFWwindow* GraphicsEngine::_get_window()
	{
		return window;
	}


	// Member operations  
	void GraphicsEngine::create_model(std::string name, const char* path)
	{
		std::shared_ptr<fgr::Model> m(new fgr::Model);
		std::promise<void>* model_loaded_signal = new std::promise<void>;
		m->loading_thread_checker = model_loaded_signal->get_future();
		fgr::Model::pending_loads.push(std::make_tuple<>(m, path, model_loaded_signal));
		models[name] = m;
	}

	void GraphicsEngine::delete_model(std::string name)
	{
		try
		{
			if (models.at(name).use_count() > 1)
			{
				std::cout << "[Figura::Error] : At GraphicsEngine::delete_model, attempted to delete a shared object!\n";
				return;
			}

			else
			{
				models.erase(name);
			}
		}

		catch (std::exception& e)
		{
			std::cout << "[Figura::Exception] : \"" << e.what() << "\" thrown at GraphicsEngine::delete_model" << std::endl;
			return;
		}
	}

	std::shared_ptr<fgr::Model>& GraphicsEngine::get_model(std::string name)
	{
		return models.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_model_names()
	{
		std::vector<std::string> names;
		for (auto& e : models) names.push_back(e.first);
		return names;
	}

	void GraphicsEngine::create_shader(std::string name, const char* vertex_shader_source, const char* fragment_shader_source, const char* geometry_shader_source)
	{
		std::shared_ptr<fgr::Shader> s(new fgr::Shader); 
		s->load_from_buffer(vertex_shader_source, fragment_shader_source, geometry_shader_source);
		shaders[name] = s;
	}

	void GraphicsEngine::delete_shader(std::string name)
	{
		try
		{
			if (shaders.at(name).use_count() > 1)
			{
				std::cout << "[Figura::Error] : At GraphicsEngine::delete_shader, attempted to delete a shared object!\n";
				return;
			}

			else
			{
				shaders.erase(name);
			}
		}
		
		catch (std::exception& e)
		{
			std::cout << "[Figura::Exception] : \"" << e.what() << "\" thrown at GraphicsEngine::delete_shader" << std::endl; 
			return;
		}
	}

	void GraphicsEngine::use_shader(std::string name)
	{
		try
		{
			current_shader = shaders.at(name);
			current_shader->use();
		}

		catch (std::exception&e)
		{
			std::cout << "[Figura::Exception] : \"" << e.what() << "\" thrown at GraphicsEngine::use_shader" << std::endl;
			return;
		}
	}

	const std::vector<std::string> GraphicsEngine::get_shader_names()
	{
		std::vector<std::string> names; 
		for (auto& e : shaders) names.push_back(e.first);
		return names;
	}

	void GraphicsEngine::create_perspective_camera(std::string name, float fov, float aspect, float zNear, float zFar)
	{
		std::unique_ptr<fgr::Camera> c(new fgr::Camera); 
		c->create_perspective(fov, aspect, zNear, zFar);
		cameras[name] = std::move(c);
	}

	void GraphicsEngine::create_orthographic_camera(std::string name, float left, float right, float bottom, float top, float near, float far)
	{
		std::unique_ptr<fgr::Camera> c(new fgr::Camera);
		c->create_ortho(left,right,bottom,top, near, far);
		cameras[name] = std::move(c);
	}

	void GraphicsEngine::delete_camera(std::string name)
	{
		cameras.erase(name);
	}

	std::unique_ptr<fgr::Camera>& GraphicsEngine::get_camera(std::string name)
	{
		return cameras.at(name); 
	}

	const std::vector<std::string> GraphicsEngine::get_camera_names()
	{
		std::vector<std::string> names; 
		for (auto& e : cameras) names.push_back(e.first); 
		return names;
	}

	void GraphicsEngine::create_directional_light(std::string name, glm::vec3 direction, glm::vec3 color)
	{
		std::unique_ptr<fgr::DirectionalLight> d_l(new fgr::DirectionalLight(direction,color)); 
		d_lights[name] = std::move(d_l);
	}

	void GraphicsEngine::delete_directional_light(std::string name)
	{
		d_lights.erase(name);
	}

	std::unique_ptr<fgr::DirectionalLight>& GraphicsEngine::get_directional_light(std::string name)
	{
		return d_lights.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_directional_light_names()
	{
		std::vector<std::string> names; 
		for (auto& e : d_lights) names.push_back(e.first); 
		return names;
	}

	void GraphicsEngine::create_point_light(std::string name, glm::vec3 position, glm::vec3 color)
	{
		std::unique_ptr<fgr::PointLight> p_l(new fgr::PointLight(position,color)); 
		p_lights[name] = std::move(p_l);
	}

	void GraphicsEngine::delete_point_light(std::string name)
	{
		p_lights.erase(name);
	}

	std::unique_ptr<fgr::PointLight>& GraphicsEngine::get_point_light(std::string name)
	{
		return p_lights.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_point_light_names()
	{
		std::vector<std::string> names; 
		for (auto& e : p_lights) names.push_back(e.first); 
		return names;
	}

	void GraphicsEngine::create_spot_light(std::string name, glm::vec3 position, glm::vec3 color, glm::vec3 orientation, float angle)
	{
		std::unique_ptr<fgr::SpotLight> s_l(new fgr::SpotLight(position, color, orientation, angle)); 
		s_lights[name] = std::move(s_l);
	}

	void GraphicsEngine::delete_spot_light(std::string name)
	{
		s_lights.erase(name);
	}

	std::unique_ptr<fgr::SpotLight>& GraphicsEngine::get_spot_light(std::string name)
	{
		return s_lights.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_spot_light_names()
	{
		std::vector<std::string> names; 
		for (auto& e : s_lights) names.push_back(e.first); 
		return names;
	}

}