#pragma once 
#include "vertexbuffer.h"
#include "indexbuffer.h"

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
	public : 	
		std::string diffuse_map_name; 
		std::string normal_map_name;
	private : 
		// OpenGL objects : 
		unsigned int vertexarray = 0;
		fgr::vertexbuffer vb;
		fgr::indexbuffer ib;

		void generate();
	public:
		void reset();
		void load(
			std::vector<fgr::vertex> verticies,
			std::vector<unsigned int>indicies,
			std::string diffuse_map_name,
			std::string normal_map_name
		);

		void Draw();
	};

}