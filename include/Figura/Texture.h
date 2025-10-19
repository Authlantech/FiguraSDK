#pragma once
#include <memory>
#include <glad/glad.h>

namespace fgr {

	class Texture
	{
	public :
		struct DATA {
		unsigned char* pixels = 0;
		int width = 0, height = 0, channels = 0;
	};
	private:
		unsigned int id = 0;
	public:		
		Texture();
		~Texture();

		bool LoadFromData(Texture::DATA data);
		void bind(const GLenum texture_unit = GL_TEXTURE0) const;
		static void unbind(const GLenum texture_unit = GL_TEXTURE0);
		static DATA LoadTextureData(const char* file_path);
	};
}