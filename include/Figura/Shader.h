#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <iostream> 
#include <fstream>

namespace fgr {

	// DEFAULT SHADERS FOR RENDERING MODELS


	static const char* model_vs = R"(
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
	)";

	static const char* model_fs = R"(
	#version 460 core

	struct DIRECTIONAL_LIGHT_DATA_LAYOUT
	{
		float x,y,z;
		float r,g,b;
	};

	struct POINT_LIGHT_DATA_LAYOUT
	{
		float x,y,z;
		float r,g,b;
	};

	struct SPOT_LIGHT_DATA_LAYOUT
	{
		float x,y,z;
		float r,g,b;
		float dx,dy,dz;
		float angle;
	};

	layout (std430, binding = 0) buffer d_lights
	{
		int directional_light_count;
		DIRECTIONAL_LIGHT_DATA_LAYOUT directional_lights[];
	};
	
	layout (std430, binding = 1) buffer p_lights
	{
		int point_light_count;
		POINT_LIGHT_DATA_LAYOUT point_lights[];
	};
	
	layout (std430, binding = 2) buffer s_lights
	{
		int spot_light_count;
		SPOT_LIGHT_DATA_LAYOUT spot_lights[];
	};

	layout (binding = 0) uniform sampler2D diffuse_map;
	layout (binding = 1) uniform sampler2D normal_map;

	in VERTEX_DATA
	{
		vec3 world_space_frag_pos;
		vec3 world_space_ver_normal;
		vec3 world_space_ver_tangent;
		vec3 world_space_ver_bittangent;
		vec2 ver_texture_coordinates;
	} frag_in;

	out vec4 final_color;
	
	void main() {

		// DECIDE THE NORMAL

		vec3 normal_vector = frag_in.world_space_ver_normal;
		vec3 normal_sample = texture(normal_map,frag_in.ver_texture_coordinates).rgb;

		if(normal_sample != vec3(0,0,0))
		{
			mat3 TBN = mat3(normalize(frag_in.world_space_ver_tangent),normalize(frag_in.world_space_ver_bittangent),normalize(frag_in.world_space_ver_normal));
			normal_vector = TBN * (normal_sample * 2.0 - 1.0);
		}

		// LIGHTING CALCULATIONS

		vec3 intermediate_value = vec3(0,0,0);
		vec3 texel_color = pow(texture(diffuse_map,frag_in.ver_texture_coordinates).rgb, vec3(2.2));
		
		// Directional Light Calculations : 
		
		for(int a = 0;a < directional_light_count;a++)
		{
			// Diffuse
			vec3 light_vector = normalize(-vec3(directional_lights[a].x, directional_lights[a].y, directional_lights[a].z));
			vec3 light_color = vec3(directional_lights[a].r, directional_lights[a].g, directional_lights[a].b);
			float diffuse_comp = max(dot(normal_vector,light_vector),0.0);
			intermediate_value += light_color * diffuse_comp * texel_color;
		}		
		
		// Point Light Calculations : 
		
		for(int a = 0;a < point_light_count;a++)
		{
			// Diffuse
			vec3 light_position = vec3(point_lights[a].x, point_lights[a].y, point_lights[a].z);
			vec3 light_color = vec3(point_lights[a].r, point_lights[a].g, point_lights[a].b);
			vec3 light_vector = normalize(light_position - frag_in.world_space_frag_pos);
			float diffuse_comp = max(dot(normal_vector,light_vector),0.0);
			intermediate_value += light_color * diffuse_comp * texel_color;
		}
		
		// Spot Light Calculations : 
		
		for(int a = 0;a < spot_light_count;a++) {
			vec3 light_pos = vec3(spot_lights[a].x,spot_lights[a].y,spot_lights[a].z);
			vec3 light_color = vec3(spot_lights[a].r, spot_lights[a].g, spot_lights[a].b);
			vec3 target_dir = normalize(vec3(spot_lights[a].dx, spot_lights[a].dy, spot_lights[a].dz));
			vec3 light_dir = normalize(frag_in.world_space_frag_pos - light_pos);
			float cos_val = dot(target_dir, light_dir);
			if(cos_val > cos(radians(spot_lights[a].angle)))
			{
				// Diffuse 
				vec3 light_vector = -light_dir;
				float diffuse_comp = max(dot(normal_vector,light_vector),0.0);
				intermediate_value += light_color * diffuse_comp * texel_color;
			}
		}
		
		float gamma = 2.2;
		final_color = vec4(pow(intermediate_value,vec3(1.0/gamma)),1.0);
	
	}
	)";


		// SHADERS FOR NORMAL RENDERING : 

		static const char* normal_vs =
			"#version 460 core\n"
			"layout(location = 0) in vec3 ver_pos;\n"
			"layout(location = 3) in vec3 ver_normal;\n"

			"uniform mat4 modelMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 projectionMatrix;\n"
			"uniform mat4 normalMatrix;\n"

			"out vec4 norm_tip;\n"

			"void main() {\n"
			"vec3 norm = normalize(mat3(normalMatrix) * ver_normal);\n"
			"norm_tip = projectionMatrix * viewMatrix * vec4(norm + vec3(modelMatrix * vec4(ver_pos, 1.0)), 1.0);\n"
			"gl_Position = projectionMatrix * viewMatrix *  modelMatrix * vec4(ver_pos, 1.0);\n"
			"}\n"
			;

		static const char* normal_gs =
			"#version 460 core\n"

			"layout (triangles) in;\n"
			"layout (line_strip, max_vertices = 12) out;\n"

			"in vec4 norm_tip[];\n"
			"out vec3 col;\n"

			"void main() {\n"

			//Render Normals : 
			"col = vec3(1.0, 1.0, 0.0);\n"

			"gl_Position = gl_in[0].gl_Position;\n"
			"EmitVertex();\n"
			"gl_Position = norm_tip[0];\n"
			"EmitVertex();\n"
			"EndPrimitive();\n"

			"gl_Position = gl_in[1].gl_Position;\n"
			"EmitVertex();\n"
			"gl_Position = norm_tip[1];\n"
			"EmitVertex();\n"
			"EndPrimitive();\n"

			"gl_Position = gl_in[2].gl_Position;\n"
			"EmitVertex();\n"
			"gl_Position = norm_tip[2];\n"
			"EmitVertex();\n"
			"EndPrimitive();\n"
			"}\n"
			;

		static const char* normal_fs =
			"#version 460 core\n"
			"in vec3 col;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"frag_color = vec4(col, 1.0);\n"
			"}\n"
			;

		// SHADERS FOR RENDERING MESHES

		static const char* mesh_vs =
			"#version 460 core\n"
			"layout(location = 0) in vec3 ver_pos;\n"
			"layout(location = 3) in vec3 ver_normal;\n"

			"uniform mat4 modelMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 projectionMatrix;\n"
			"uniform mat4 normalMatrix;\n"			

			"void main() {\n"		
			"gl_Position = projectionMatrix * viewMatrix *  modelMatrix * vec4(ver_pos, 1.0);\n"
			"}\n"
			;

		static const char* mesh_gs =
			"#version 460 core\n"

			"layout (triangles) in;\n"
			"layout (line_strip, max_vertices = 6) out;\n"

			"void main() {\n"
				"gl_Position = gl_in[0].gl_Position;\n"
				"EmitVertex();\n"
				"gl_Position = gl_in[1].gl_Position;\n"
				"EmitVertex();\n"
				"EndPrimitive();\n"

				"gl_Position = gl_in[1].gl_Position;\n"
				"EmitVertex();\n"
				"gl_Position = gl_in[2].gl_Position;\n"
				"EmitVertex();\n"
				"EndPrimitive();\n"

				"gl_Position = gl_in[0].gl_Position;\n"
				"EmitVertex();\n"
				"gl_Position = gl_in[2].gl_Position;\n"
				"EmitVertex();\n"
				"EndPrimitive();\n"
			"}\n"
			;

		static const char* mesh_fs =
			"#version 460 core\n"
			"in vec3 col;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"frag_color = vec4(0.0, 1.0, 0.0, 1.0);\n"
			"}\n"
			;



	class Shader {
	private:
		unsigned int program = 0;
		unsigned int vshader = 0; 
		unsigned int fshader = 0; 
		unsigned int gshader = 0; 
	public:
		friend class Scene;
		friend class Camera;
		friend class Model;

        Shader();
        ~Shader();

		void load_from_buffer(const char* vertex_shader, const char* fragment_shader,const char* geometry_shader = nullptr);
		void load_from_file(const char* vertex_shader_path,const char* fragment_shader_path,const char* geometry_shader_path = nullptr);
	private :
		void use();
		void uniformmat4f(const char* name, glm::mat4 val);
		void uniformfloat(const char* name, float val);
		void uniformvec3(const char* name, float x, float y, float z);
		void uniformint(const char* name, int v);

	};


}