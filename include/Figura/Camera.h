#pragma once 
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	protected:
		glm::mat4 viewMatrix = glm::mat4(1.f);
		glm::mat4 projectionMatrix = glm::mat4(1.f);

		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 oreintation = glm::vec3(0.f, 0.f, -1.f);

		glm::mat4 get_viewMatrix();
		glm::mat4 get_projectionMatrix();
	public:
		friend class GraphicsEngine;

		Camera();
		virtual ~Camera() = 0;

		void set_position(glm::vec3 pos);
		void face(glm::vec3 target);

		glm::vec3 get_oreintation();
		glm::vec3 get_position();
	};


	class OrthoCamera : public Camera
	{
	public : 
		OrthoCamera(OrthographicAttribs attribs);
	};

	class PerspectiveCamera : public Camera
	{
	public : 
		PerspectiveCamera(PerspectiveAttribs attribs);
	};

}
