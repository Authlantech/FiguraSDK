#pragma once 

#include <Figura/camera.h>
#include <Figura/light.h>
#include <Figura/model.h>
#include <Figura/shader.h>
#include <GLFW/glfw3.h>


namespace fgr
{

	class GraphicsEngine
	{
	private :
		GLFWwindow* window = nullptr;

		std::shared_ptr<fgr::Shader> current_shader = nullptr;
		std::unordered_map<std::string, std::shared_ptr<fgr::Model>>			  models; 
		std::unordered_map<std::string, std::shared_ptr<fgr::Shader>>			  shaders;
		std::unordered_map<std::string, std::unique_ptr<fgr::Camera>>			  cameras;
		std::unordered_map<std::string, std::unique_ptr<fgr::DirectionalLight>>	  d_lights; 
		std::unordered_map<std::string, std::unique_ptr<fgr::PointLight>>		  p_lights; 
		std::unordered_map<std::string, std::unique_ptr<fgr::SpotLight>>		  s_lights;
	public : 	
		friend class fgr::Model;
		friend class fgr::Camera;

		// Engine and window functions 
		void init_engine(int window_width,int window_height,const char* window_title,int opengl_version_major = 4,int opengl_version_minor = 6,int opengl_profile = GLFW_OPENGL_COMPAT_PROFILE);
		void clear_window(float red,float green,float blue,float alpha);
		void update_window();
		bool window_is_open();

		// Member operations  
		void create_model(std::string name, const char*path);
		void delete_model(std::string name);
		std::shared_ptr<fgr::Model>& get_model(std::string name);
		const std::vector<std::string> get_model_names();

		void create_shader(std::string name, const char* vertex_shader_source, const char* fragment_shader_source, const char* geometry_shader_source = nullptr); 
		void delete_shader(std::string name); 
		void use_shader(std::string name);
		const std::vector<std::string> get_shader_names();

		void create_perspective_camera(std::string name, float fov, float aspect, float zNear, float zFar); 
		void create_orthographic_camera(std::string name, float left, float right, float bottom, float top, float near, float far); 
		void delete_camera(std::string name); 
		std::unique_ptr<fgr::Camera>& get_camera(std::string name); 
		const std::vector<std::string> get_camera_names(); 

		void create_directional_light(std::string name,glm::vec3 direction,glm::vec3 color); 
		void delete_directional_light(std::string name);
		std::unique_ptr<fgr::DirectionalLight>& get_directional_light(std::string name); 
		const std::vector<std::string> get_directional_light_names(); 

		void create_point_light(std::string name, glm::vec3 position, glm::vec3 color); 
		void delete_point_light(std::string name);
		std::unique_ptr<fgr::PointLight>& get_point_light(std::string name);
		const std::vector<std::string> get_point_light_names();

		void create_spot_light(std::string name, glm::vec3 position, glm::vec3 color, glm::vec3 orientation, float angle);
		void delete_spot_light(std::string name);
		std::unique_ptr<fgr::SpotLight>& get_spot_light(std::string name);
		const std::vector<std::string> get_spot_light_names(); 


		GLFWwindow* _get_window();
	};

	extern GraphicsEngine graphic_engine;
}