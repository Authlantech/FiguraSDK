#include <Figura/camera.h>
using namespace fgr;

void Camera::create_perspective(float fov, float aspect, float zNear, float zFar)
{
	projectionMatrix = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	viewMatrix = glm::lookAt(position, position + oreintation, glm::vec3(0.f, 1.f, 0.f));
}

void Camera::create_ortho(float left, float right, float bottom, float top,float near,float far)
{
	projectionMatrix = glm::ortho(left, right, bottom,top,near,far);
	viewMatrix = glm::lookAt(position, position + oreintation, glm::vec3(0.f, 1.f, 0.f));
}


void Camera::set_position(glm::vec3 pos)
{
	position = pos; 
	viewMatrix = glm::lookAt(position, position + oreintation, glm::vec3(0.f, 1.f, 0.f));
}

void Camera::face(glm::vec3 target)
{
	oreintation = glm::normalize(target - position);
	viewMatrix = glm::lookAt(position, position + oreintation, glm::vec3(0.f, 1.f, 0.f));
}

glm::vec3 Camera::get_oreintation()
{
	return oreintation;
}

glm::vec3 Camera::get_position()
{
	return position;
}

glm::mat4 Camera::get_viewMatrix()
{
	return viewMatrix;
}

glm::mat4 Camera::get_projectionMatrix()
{
	return projectionMatrix;
}

void Camera::use(Shader shader)
{
	shader.uniformmat4f("projectionMatrix", projectionMatrix); 
	shader.uniformmat4f("viewMatrix", viewMatrix);
}
