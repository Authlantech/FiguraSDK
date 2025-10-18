#pragma once
#include <glad/glad.h>

namespace fgr {

	class IndexBuffer {
		unsigned int id = 0;
		unsigned int indices_count = 0;
	public:
		IndexBuffer();
		~IndexBuffer();

		void bind() const;
		void data(const unsigned int* indices, unsigned int size);
		unsigned int get_count() const;

	};

}