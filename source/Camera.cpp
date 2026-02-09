#include <Figura/camera.h>

using namespace fgr;

Camera::Camera()
{
	face({ 0,0,-1 });
}

Camera::~Camera()
{

}

OrthoCamera::OrthoCamera(OrthographicAttribs attribs) : Camera()
{
	projectionMatrix = glm::ortho(attribs.left, attribs.right, attribs.bottom, attribs.top);
}

PerspectiveCamera::PerspectiveCamera(PerspectiveAttribs attribs) : Camera()
{
	projectionMatrix = glm::perspective(glm::radians(attribs.fov), attribs.aspect, attribs.near, attribs.far);
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

