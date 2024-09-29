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
		"\n"
		"layout (location = 0) in vec3 _vpos;\n"
		"layout (location = 2) in vec2 _vtexcoords;\n"
		"layout (location = 3) in vec3 _vnormal;\n"
		"\n"
		"out vec2 _ftexcoords;\n"
		"out vec3 _fnormal;\n"
		"out vec3 _fragPos;\n"
		"\n"
		"uniform mat4 modelMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"uniform mat4 projectionMatrix;\n"
		"uniform mat4 normalMatrix;\n"
		"\n"
		"void main() {\n"
		"\tgl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(_vpos,1.0);\n"
		"\t_ftexcoords = _vtexcoords;\n"
		"\t_fnormal =  mat3(normalMatrix) * _vnormal;\n"
		"\t_fragPos = vec3(modelMatrix * vec4(_vpos,1.0));\n"
		"}\n";

	static const char* default_fs =
		"#version 460 core\n"
		"\n"
		// INPUTS : 
		"\n"
		"in vec2 _ftexcoords;\n"
		"in vec3 _fnormal;\n"
		"in vec3 _fragPos;\n"
		"\n"
		"vec3 normVector = normalize(_fnormal);\n"
		"\n"
		// Directional Light : 
		"\n"
		"struct D_LIGHT {\n"
			"\tfloat x,y,z;\n"
			"\tfloat r,g,b;\n"
		"};\n"
		"\n"
		"layout (std430, binding = 0) buffer d_lights {\n"
			"\tint d_count;\n"		
			"\tD_LIGHT d_light[];\n"		
		"};\n"
		"\n"
		//Point Light : 
		"\n"
		"struct P_LIGHT {\n"
			"\tfloat x,y,z;\n"
			"\tfloat r,g,b;\n"
		"};\n"
		"\n"
		"layout (std430, binding = 1) buffer p_lights {\n"
			"\tint p_count;\n"
			"\tP_LIGHT p_light[];\n"
		"};\n"
		"\n"
		// Spot Light : 
		"\n"
		"struct S_LIGHT {\n"
			"\tfloat x,y,z;\n"
			"\tfloat r,g,b;\n"
			"\tfloat dx,dy,dz;\n"
			"\tfloat angle;\n"
		"};\n"
		"\n"
		"layout (std430, binding = 2) buffer s_lights {\n"
			"\tint s_count;\n"
			"\tS_LIGHT s_light[];\n"
		"};\n"
		"\n"

		//Light Maps : 
		"layout (binding = 0) uniform sampler2D diffusemap;\n"
		"layout (binding = 1) uniform sampler2D specularmap;\n"
		"layout (binding = 2) uniform sampler2D metalnessmap;\n"

		"uniform vec3 viewer_position;\n"

		"\n"
		// OUTPUTS : 
		"\n"
		"out vec4 final_color;\n"
		"\n"
		// MAIN 
		"void main() {\n"

		// LIGHTING CALCULATIONS : 

		"vec3 result = vec3(0.f, 0.f, 0.f);\n"

		//Ambient 
		"vec3 ambientcol = vec3(1.0, 1.0, 1.0);\n"
		"float ambientsth = 0.03;\n"
		"result += ambientcol * ambientsth * texture(diffusemap,_ftexcoords).rgb;\n"

		// Directional Light Calculations : 

		"for(int a = 0;a < d_count;a++) {\n"
			// Diffuse 
			"\tvec3 lightvector = normalize(-vec3(d_light[a].x, d_light[a].y, d_light[a].z));\n"
			"\tfloat diffsth = max(dot(normVector,lightvector),0.0);\n"
			"\tvec3 diffres = vec3(d_light[a].r, d_light[a].g, d_light[a].b) * diffsth * texture(diffusemap,_ftexcoords).rgb;\n"
			"\tresult += diffres;\n"
			// Specular
			"vec3 viewDir = normalize(viewer_position - _fragPos);\n"
			"vec3 reflected = reflect(-lightvector,normVector);\n"
			"float specsth = pow(max(dot(viewDir,reflected), 0.0), 10.f);\n"
			"vec3 specres = vec3(d_light[a].r, d_light[a].g, d_light[a].b) * specsth * texture(specularmap,_ftexcoords).rgb;\n"
			"result += specres;\n"
		"}\n"
		"\n"
		// Point Light Calculations : 
		"\n"
		"for(int a = 0;a < p_count;a++) {\n"

			// Diffuse
			"\tvec3 lightvector = normalize(vec3(p_light[a].x, p_light[a].y, p_light[a].z) - _fragPos);\n"
			"\tfloat diffsth = max(dot(normVector,lightvector),0.0);\n"
			"\tvec3 diffres = vec3(p_light[a].r, p_light[a].g, p_light[a].b) * diffsth * texture(diffusemap,_ftexcoords).rgb;\n"
			"\tresult += diffres;\n"

			// Specular
			"vec3 viewDir = normalize(viewer_position - _fragPos);\n"
			"vec3 reflected = reflect(-lightvector,normVector);\n"
			"float specsth = pow(max(dot(viewDir,reflected), 0.0), 10.f);\n"
			"vec3 specres = vec3(d_light[a].r, d_light[a].g, d_light[a].b) * specsth * texture(specularmap,_ftexcoords).rgb;\n"
			"result += specres;\n"

		"}\n"
		"\n"
		// Spot Light Calculations : 
		"\n"
		"for(int a = 0;a < s_count;a++) {\n"
			"\tvec3 light_pos = vec3(s_light[a].x,s_light[a].y,s_light[a].z);\n"
			"\tvec3 target_dir = normalize(vec3(s_light[a].dx, s_light[a].dy, s_light[a].dz));\n"
			"\tvec3 light_dir = normalize(_fragPos - light_pos);\n"
			"\tfloat cos_val = dot(target_dir, light_dir);\n"
			"\tif(cos_val > cos(radians(s_light[a].angle))) {\n"
				// Diffuse 
				"\t\tvec3 lightvector = -light_dir;\n"
				"\t\tfloat diffsth = max(dot(normVector,lightvector),0.0);\n"
				"\t\tvec3 diffres = vec3(s_light[a].r, s_light[a].g, s_light[a].b) * diffsth * texture(diffusemap,_ftexcoords).rgb;\n"
				"\t\tresult += diffres;\n"

				// Specular
				"vec3 viewDir = normalize(viewer_position - _fragPos);\n"
				"vec3 reflected = reflect(-lightvector,normVector);\n"
				"float specsth = pow(max(dot(viewDir,reflected), 0.0), 10.f);\n"
				"vec3 specres = vec3(d_light[a].r, d_light[a].g, d_light[a].b) * specsth * texture(specularmap,_ftexcoords).rgb;\n"
				"result += specres;\n"
			"}\n"
		"}\n"
		"\n"
		"final_color = vec4(result,1.0);\n"
		"\n"
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