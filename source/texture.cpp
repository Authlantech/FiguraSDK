#include <Figura/texture.h>

using namespace fgr;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() {
	glGenTextures(1,&id);
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture() {
	glDeleteTextures(1,&id);
}

void Texture::bind(const GLenum texture_unit) const
{
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D,id);
}

void Texture::unbind(const GLenum texture_unit)
{
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::LoadFromFile(const char* path) const
{
	unsigned char* pixels = nullptr;
	int width = 0,height = 0,nrChannels = 0;

	stbi_set_flip_vertically_on_load(true);
	pixels = stbi_load(path, &width, &height, &nrChannels,3);
	if (pixels) {
		bind();
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(pixels);
		return true;
	}
	else {
		return false;
	}
}

void Texture::LoadFromBuffer(const void *buffer,int width,int height) const {
	bind();
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,buffer);
	glGenerateMipmap(GL_TEXTURE_2D);
}
