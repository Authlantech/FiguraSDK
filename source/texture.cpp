#include <Figura/texture.h>

using namespace fgr;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void Texture::generate()
{
	if (pixels != nullptr)
	{
		glGenTextures(1, &_id);
		bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(pixels);
		pixels = nullptr;		
	}
}

void Texture::reset()
{
	if (_id != 0)
	{		
		glDeleteTextures(1, &_id);
		_id = 0;
	}

	if (pixels != nullptr)
	{		
		stbi_image_free(pixels); 
		pixels = nullptr;
	}	
}

void Texture::bind(GLenum texture_unit)
{	
	if (_id != 0)
	{
		glActiveTexture(texture_unit);
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	else
	{
		glActiveTexture(texture_unit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Texture::unbind(GLenum texture_unit)
{
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadFromFile(const char* path)
{	
	stbi_set_flip_vertically_on_load(true);
	pixels = stbi_load(path, &width, &height, &nrChannels,3); 
}

bool Texture::ready_for_rendering()
{
	return _id != 0; 
}

bool Texture::pixels_stored_in_ram()
{
	return pixels;
}