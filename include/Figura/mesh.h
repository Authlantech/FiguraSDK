#pragma once 
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "texture.h"
#include <Figura/shader.h>

#include <iostream> 
#include <vector> 

namespace fgr {

	class Mesh
	{
	private:
		unsigned int vertexarray;
		vertexbuffer vb;
		indexbuffer ib;
		Texture _diffusemap;
		GLenum mode = GL_TRIANGLES;

		std::vector<fgr::vertex> vertices; 
		std::vector<unsigned int> indices;
	public:
		void generate();
		void destroy();

		void load(
			vertex* vertices, unsigned int vertices_size,
			unsigned int* indices, unsigned int indices_size,
			Texture diffusemap
		);

		std::vector<fgr::vertex> get_vertices(); 
		std::vector<unsigned int> get_indices();

		void set_mode(GLenum mode);
		void Draw(Shader shader);
	};

}