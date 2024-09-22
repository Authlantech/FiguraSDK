#pragma once
#include <glad/glad.h>

namespace fgr {

	class Texture
	{
	private:
		unsigned int id;
		int width, height, nrChannels;
		int _loaded = 0;
	public:
		void generate();
		void destroy();

		void bind(GLenum texture_unit = GL_TEXTURE0);
		void load(const char* path);

		unsigned int give_id();

		int loaded();
	};
}