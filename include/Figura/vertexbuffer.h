#pragma once
#include <glad/glad.h>

namespace fgr {

	struct vertex
	{
		float pos[3];
		float color[4];
		float texture_coordinates[2];
		float normals[3];
	};


	class vertexbuffer {
	private:
		unsigned int id;
	public:
		void generate();
		void destroy();

		void bind();
		void data(vertex* data, unsigned int size);
		void set_layout();
	};

}