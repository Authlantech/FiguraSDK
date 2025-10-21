#pragma once
#include <glad/glad.h>

namespace fgr {

	struct Vertex
	{
		float position[3];
		float color[4];
		float texture_coordinates[3];
		float normal[3];
		float tangent[3]; 
		float bittangent[3];
	};


	class VertexBuffer {
		unsigned int id = 0;
	public:
		friend class Mesh;
		VertexBuffer();
		~VertexBuffer();
	private :
		void bind() const ;
		void data(const Vertex* vertices,unsigned int size) const;
		void set_layout() const;
	};

}