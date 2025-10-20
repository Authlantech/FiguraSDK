#pragma once 
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

namespace fgr {

	struct PerspectiveAttribs
	{
		float fov; 
		float aspect;
		float near; 
		float far; 
	};

	struct OrthographicAttribs
	{
		float left; 
		float right; 
		float bottom; 
		float top; 
		float near; 
		float far;
	};

	class Camera {
	private:
		glm::mat4 viewMatrix = glm::mat4(1.f);
		glm::mat4 projectionMatrix = glm::mat4(1.f);

		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 oreintation = glm::vec3(0.f, 0.f, -1.f);

		// Attribs : 
		PerspectiveAttribs p_attribs = { 0 };
		OrthographicAttribs o_attribs = { 0 };

	public:
		friend class Scene;

		Camera() = default;
		~Camera() = default;

		void configure_perspective(float fov, float aspect, float zNear, float zFar);
		void configure_ortho(float left,float right,float bottom,float top,float near,float far);

		void set_position(glm::vec3 pos);
		void face(glm::vec3 target);

		glm::vec3 get_oreintation();
		glm::vec3 get_position();
	private :
		glm::mat4 get_viewMatrix(); 
		glm::mat4 get_projectionMatrix();

		PerspectiveAttribs give_perpective_attribs(); 
		OrthographicAttribs give_orthographic_attribs();

		void use(std::shared_ptr<Shader> shader);
	};

}
