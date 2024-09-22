#include <Figura/indexbuffer.h>

using namespace fgr;

void indexbuffer::generate()
{
	glGenBuffers(1, &id);
}

void indexbuffer::destroy()
{
	glDeleteBuffers(1, &id);
}

void indexbuffer::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void indexbuffer::data(unsigned int* indices
	, unsigned int size)
{
	bind(); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices,GL_STATIC_DRAW); 
	indices_count = size / sizeof(unsigned int);
}

unsigned int indexbuffer::get_count()
{
	return indices_count;
}
