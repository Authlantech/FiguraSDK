#pragma once 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

namespace fgr {

	class Camera {
	private:
		glm::mat4 viewMatrix = glm::mat4(1.f);
		glm::mat4 projectionMatrix = glm::mat4(1.f);

		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 oreintation = glm::vec3(0.f, 0.f, -1.f);

	public:
		void create_perspective(float fov, float aspect, float zNear, float zFar);
		void create_ortho(float left,float right,float bottom,float top,float near,float far);
		void set_position(glm::vec3 pos);
		void face(glm::vec3 target);

		glm::vec3 get_oreintation();
		glm::vec3 get_position();

		glm::mat4 get_viewMatrix(); 
		glm::mat4 get_projectionMatrix();

		void use(Shader shader);
	};

}