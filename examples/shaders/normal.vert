#version 460 core
layout(location = 0) in vec3 ver_pos;
layout(location = 3) in vec3 ver_normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

out vec4 norm_tip;

void main() {
	vec3 norm = normalize(mat3(normalMatrix) * ver_normal);
	norm_tip = projectionMatrix * viewMatrix * vec4(norm + vec3(modelMatrix * vec4(ver_pos, 1.0)), 1.0);
	gl_Position = projectionMatrix * viewMatrix *  modelMatrix * vec4(ver_pos, 1.0);
}
