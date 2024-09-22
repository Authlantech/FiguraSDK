#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <iostream> 
#include <fstream>

namespace fgr {

	// DEFAULT SHADERS FOR RENDERING MODELS 

	static const char* default_vs =
		"#version 460 core\n"

		"layout (location = 0) in vec3 _vpos;\n"
		"layout (location = 2) in vec2 _vtexcoords;\n"
		"layout (location = 3) in vec3 _vnormal;\n"

		"out vec2 _ftexcoords;\n"
		"out vec3 _fnormal;\n"
		"out vec3 _fragPos;\n"

		"uniform mat4 modelMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"uniform mat4 projectionMatrix;\n"
		"uniform mat4 normalMatrix;\n"

		"void main() {\n"
		"gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(_vpos,1.0);\n"
		"_ftexcoords = _vtexcoords;\n"
		"_fnormal =  mat3(normalMatrix) * _vnormal;\n"
		"_fragPos = vec3(modelMatrix * vec4(_vpos,1.0));\n"
		"}\n";

	static const char* default_fs =
		"#version 460 core\n"

		// INPUTS : 

		"in vec2 _ftexcoords;\n"
		"in vec3 _fnormal;\n"
		"in vec3 _fragPos;\n"

		"vec3 normVector = normalize(_fnormal);\n"

		// Directional Light : 

		"struct D_LIGHT {\n"
			"float x,y,z;\n"
			"float r,g,b;\n"
		"};\n"

		"layout (std430, binding = 0) buffer d_lights {\n"
			"int d_count;\n"		
			"D_LIGHT d_light[];\n"		
		"};\n"

		//Point Light : 

		"struct P_LIGHT {\n"
			"float x,y,z;\n"
			"float r,g,b;\n"
		"};\n"

		"layout (std430, binding = 1) buffer p_lights {\n"
			"int p_count;\n"
			"P_LIGHT p_light[];\n"
		"};\n"

		// Spot Light : 

		"struct S_LIGHT {\n"
			"float x,y,z;\n"
			"float r,g,b;\n"
			"float dx,dy,dz;\n"
			"float angle;\n"
		"};\n"

		"layout (std430, binding = 2) buffer s_lights {\n"
			"int s_count;\n"
			"S_LIGHT s_light[];\n"
		"};\n"

		//Light Maps : 
		"uniform sampler2D diffusemap;\n"
		"uniform int dmloaded;\n"

		// OUTPUTS : 

		"out vec4 final_color;\n"

		// MAIN 
		"void main() {\n"
		
		"vec3 modelcolor = vec3(0.3, 0.3, 0.3);\n"
		"if(dmloaded == 1) {\n"
			"modelcolor = vec3(texture(diffusemap,_ftexcoords));\n"
		"}\n"
		
		"vec3 result = vec3(0.0, 0.0, 0.0);\n"

		//AMBIENT CALCULATIONS : 

		"vec3 ambientcol = vec3(1.0, 1.0, 1.0);\n"
		"float ambientsth = 0.03;\n"
		"result += ambientcol * ambientsth * modelcolor;\n"

		// DIFFUSE CALCULATIONS : 

		// Directional Light Calculations : 

		"for(int a = 0;a < d_count;a++) {\n"
			"vec3 lightvector = normalize(-vec3(d_light[a].x, d_light[a].y, d_light[a].z));\n"
			"float diffsth = max(dot(normVector,lightvector),0.0);\n"
			"vec3 diffres = vec3(d_light[a].r, d_light[a].g, d_light[a].b) * diffsth * modelcolor;\n"
			"result += diffres;\n"
		"}\n"

		// Point Light Calculations : 

		"for(int a = 0;a < p_count;a++) {\n"
			"vec3 lightvector = normalize(vec3(p_light[a].x, p_light[a].y, p_light[a].z) - _fragPos);\n"
			"float diffsth = max(dot(normVector,lightvector),0.0);\n"
			"vec3 diffres = vec3(p_light[a].r, p_light[a].g, p_light[a].b) * diffsth * modelcolor;\n"
			"result += diffres;\n"
		"}\n"

		// Spot Light Calculations : 

		"for(int a = 0;a < s_count;a++) {\n"
			"vec3 light_pos = vec3(s_light[a].x,s_light[a].y,s_light[a].z);\n"
			"vec3 target_dir = normalize(vec3(s_light[a].dx, s_light[a].dy, s_light[a].dz));\n"
			"vec3 light_dir = normalize(_fragPos - light_pos);\n"
			"float cos_val = dot(target_dir, light_dir);\n"
			"if(cos_val > cos(radians(s_light[a].angle))) {\n"
				"vec3 lightvector = -light_dir;\n"
				"float diffsth = max(dot(normVector,lightvector),0.0);\n"
				"vec3 diffres = vec3(s_light[a].r, s_light[a].g, s_light[a].b) * diffsth * modelcolor;\n"
				"result += diffres;\n"
			"}\n"
		"}\n"

		"final_color = vec4(result,1.0);\n"

		"}\n";

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
		unsigned int program;
		unsigned int vshader; // vertex shader
		unsigned int fshader; // fragment shader
		unsigned int gshader; // geometry shader
	public:

		void create();	
		void destroy();

		void load_from_buffer(const char* vertex_shader, const char* fragment_shader,const char* geometry_shader = nullptr);
		void load_from_file(const char* vertex_shader_path,const char* fragment_shader_path,const char* geometry_shader_path = nullptr);

		void use();

		void uniformmat4f(const char* name, glm::mat4 val);
		void uniformfloat(const char* name, float val);
		void uniformvec3(const char* name, float x, float y, float z);
		void uniformint(const char* name, int v);

	};


}