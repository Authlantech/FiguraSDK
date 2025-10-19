#include <Figura/indexbuffer.h>

using namespace fgr;

IndexBuffer::IndexBuffer()
{
	glGenBuffers(1,&id);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1,&id);
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::data(const unsigned int* indices,unsigned int size)
{
	bind(); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices,GL_STATIC_DRAW); 
	indices_count = size / sizeof(unsigned int);
}

unsigned int IndexBuffer::get_count() const
{
	return indices_count;
}
