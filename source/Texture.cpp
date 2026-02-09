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

TEXTURE_DATA Texture::LoadTextureData(const char* file_path) {
	TEXTURE_DATA data;
	stbi_set_flip_vertically_on_load(true);
	data.pixels = stbi_load(file_path,&data.width,&data.height,&data.channels,4);
	return data;
}

bool Texture::LoadFromData(TEXTURE_DATA data) {
	if (data.pixels == NULL) {
		return false;
	}
	else {
		bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		return true;
	}
}