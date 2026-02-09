#version 460 core
layout(location = 0) in vec3 ver_pos;
layout(location = 3) in vec3 ver_normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

void main() {
	gl_Position = projectionMatrix * viewMatrix *  modelMatrix * vec4(ver_pos, 1.0);
}
