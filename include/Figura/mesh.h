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
		std::string albedo_map_name;
		std::string normal_map_name;
		std::string metalness_map_name;
		std::string roughness_map_name;
		std::string ao_map_name;
	private : 
		// OpenGL objects : 
		unsigned int vertexarray = 0;
		fgr::vertexbuffer vb;
		fgr::indexbuffer ib;

		void generate();
	public:
		Mesh() {}; 
		~Mesh() { reset(); };
		void reset();
		void load(
			std::vector<fgr::vertex> verticies,
			std::vector<unsigned int>indicies,
			std::string albedo_map_name,
			std::string normal_map_name,
			std::string metalnessmapname,
			std::string roughnessmapname,
			std::string aomapname
		);

		void Render();
	};

}