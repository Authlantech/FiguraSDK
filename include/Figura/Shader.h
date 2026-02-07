#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <iostream> 
#include <fstream>

namespace fgr {

	class Shader {
	private:
		unsigned int program = 0;
		unsigned int vshader = 0; 
		unsigned int fshader = 0; 
		unsigned int gshader = 0; 
	public:
		friend class GraphicsEngine;
		Shader();
        ~Shader();
		void LoadFromFile(const char* vertex_shader, const char* fragment_shader,const char* geometry_shader = nullptr);
	private :
		void use();
		void uniformmat4f(const char* name, glm::mat4 val);
		void uniformfloat(const char* name, float val);
		void uniformvec3(const char* name, float x, float y, float z);
		void uniformint(const char* name, int v);

	};


}