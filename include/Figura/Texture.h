#pragma once
#include <memory>
#include <glad/glad.h>

namespace fgr {


	struct TEXTURE_DATA {
		unsigned char* pixels = 0;
		int width = 0, height = 0, channels = 0;
	};

	class Texture
	{
		unsigned int id = 0;
	public:		
		Texture();
		~Texture();

		bool LoadFromData(TEXTURE_DATA data);
		void bind(const GLenum texture_unit = GL_TEXTURE0) const;
		static void unbind(const GLenum texture_unit = GL_TEXTURE0);
		static TEXTURE_DATA LoadTextureData(const char* file_path);
	};


}