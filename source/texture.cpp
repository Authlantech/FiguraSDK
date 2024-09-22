#include <Figura/texture.h>

using namespace fgr;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void Texture::generate()
{
	glGenTextures(1, &id);	
}

void Texture::destroy()
{
	glDeleteTextures(1, &id);
}

void Texture::bind(GLenum texture_unit)
{	
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D,id);
}

void Texture::load(const char* path)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels,3); 
	if (data) {
		bind(); 		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); 
		glGenerateMipmap(GL_TEXTURE_2D);
		_loaded = 1;
	}

	else {
		_loaded = 0;
	}
	
	stbi_image_free(data);
}

int Texture::loaded()
{
	return _loaded;
}

unsigned int Texture::give_id()
{
	return id;
}