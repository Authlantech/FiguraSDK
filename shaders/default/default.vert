#version 460 core

layout (location = 0) in vec3 local_space_ver_pos;
layout (location = 2) in vec2 ver_texture_coords;
layout (location = 3) in vec3 local_space_ver_normal;
layout (location = 4) in vec3 local_space_ver_tangent;
layout (location = 5) in vec3 local_space_ver_bittangent;

out VERTEX_DATA
{
	vec3 world_space_frag_pos;
	vec3 world_space_ver_normal;
	vec3 world_space_ver_tangent;
	vec3 world_space_ver_bittangent;
	vec2 ver_texture_coordinates;

} ver_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(local_space_ver_pos,1.0);
	ver_out.ver_texture_coordinates = ver_texture_coords;
	ver_out.world_space_ver_normal = mat3(normalMatrix) * local_space_ver_normal;
	ver_out.world_space_ver_tangent = mat3(normalMatrix) * local_space_ver_tangent;
	ver_out.world_space_ver_bittangent = mat3(normalMatrix) * local_space_ver_bittangent;
	ver_out.world_space_frag_pos = vec3(modelMatrix * vec4(local_space_ver_pos,1.0));
}
