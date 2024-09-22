#include <Figura/vertexbuffer.h>

using namespace fgr;

void vertexbuffer::generate()
{
	glGenBuffers(1, &id); 
}

void vertexbuffer::destroy()
{
	glDeleteBuffers(1, &id); 
}

void vertexbuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void vertexbuffer::data(vertex* data, unsigned int size)
{
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void vertexbuffer::set_layout()
{
	bind();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(vertex), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(vertex), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(vertex), (void*)(sizeof(float) * 7));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(vertex), (void*)(sizeof(float) * 9));
}
