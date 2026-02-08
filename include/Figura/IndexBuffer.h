#pragma once
#include <glad/glad.h>

namespace fgr {

	class IndexBuffer {
	protected: 
		unsigned int id = 0;
		unsigned int indices_count = 0;
		
		void bind() const;
		void data(const unsigned int* indices, unsigned int size);
		unsigned int get_count() const;
	public:
		friend class Mesh;
		IndexBuffer();
		~IndexBuffer();
	};

}