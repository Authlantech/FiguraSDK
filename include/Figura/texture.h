#pragma once
#include <memory>
#include <glad/glad.h>

namespace fgr {

	class Texture
	{
		unsigned int id = 0;
	public:		
		Texture();
		~Texture();

		bool LoadFromFile(const char* path) const;
		void LoadFromBuffer(const void* buffer,int width,int height) const;
		void bind(const GLenum texture_unit = GL_TEXTURE0) const;
		static void unbind(const GLenum texture_unit = GL_TEXTURE0);
		
	};
}