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

		// Internal Data
		std::vector<fgr::vertex> verticies; 
		std::vector<unsigned int> indicies; 		

		// OpenGL objects : 
		unsigned int vertexarray = 0;
		vertexbuffer vb;
		indexbuffer ib;
		Texture diffusemap;		
		Texture normalmap;			

		void generate();
	public:

		void reset();
		void load(
			std::vector<fgr::vertex> verticies,
			std::vector<unsigned int> indicies,
			Texture diffusemap,
			Texture normalmap
		);

		void Draw();
	};

}