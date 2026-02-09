#version 460 core

layout (binding = 0) uniform sampler2D albedo_map;
layout (binding = 1) uniform sampler2D normal_map;
layout (binding = 2) uniform sampler2D metallic_map; 
layout (binding = 2) uniform sampler2D roughness_map; 
layout (binding = 2) uniform sampler2D ao_map; 

in VERTEX_DATA
{
	vec3 world_space_frag_pos;
	vec3 world_space_ver_normal;
	vec3 world_space_ver_tangent;
	vec3 world_space_ver_bittangent;
	vec2 ver_texture_coordinates;
} frag_in;

out vec4 frag_color;

void main() {
	frag_color = texture(albedo_map, frag_in.ver_texture_coordinates);
}