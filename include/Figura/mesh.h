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

		Texture diffusemap;		

		GLenum mode = GL_TRIANGLES;
	public:
		void generate();
		void destroy();

		void load(
			vertex* vertices, unsigned int vertices_size,
			unsigned int* indices, unsigned int indices_size,
			Texture diffusemap
		);

		void update_vertices(vertex* vertices, unsigned int vertices_size);
		void update_indices(unsigned int* indices, unsigned int indices_size);

		void update_diffuse_map(Texture diffusemap);		
		void set_mode(GLenum mode);

		void Draw(Shader shader);
	};

}