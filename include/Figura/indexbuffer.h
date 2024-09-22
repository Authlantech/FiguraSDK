#pragma once
#include <glad/glad.h>

namespace fgr {

	class indexbuffer {
	private:
		unsigned int id;
		unsigned int indices_count = 0;
	public:
		void generate();
		void destroy();

		void bind();
		void data(unsigned int* indices, unsigned int size);

		unsigned int get_count();

	};

}