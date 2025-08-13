#include <Figura/mesh.h>

using namespace fgr;

void Mesh::generate()
{
	glGenVertexArrays(1, &vertexarray); 
	glBindVertexArray(vertexarray); 
	vb.generate(); 
	vb.set_layout(); 

	ib.generate(); 
	ib.bind(); 
}

void Mesh::destroy()
{
	vb.destroy(); 
	ib.destroy(); 
	glDeleteVertexArrays(1,&vertexarray); 
}

void Mesh::load(
	vertex* vertices, unsigned int vertices_size,
	unsigned int* indices, unsigned int indices_size,
	Texture diffusemap,
	Texture normalmap
	)
{
	glBindVertexArray(vertexarray); 
	vb.data(vertices, vertices_size); 
	ib.data(indices, indices_size); 

	this->diffusemap = diffusemap;
	this->normalmap = normalmap;
}

void Mesh::update_vertices(vertex* vertices, unsigned int vertices_size)
{
	glBindVertexArray(vertexarray);
	vb.data(vertices, vertices_size);
}

void Mesh::update_indices(unsigned int* indices, unsigned int indices_size)
{
	glBindVertexArray(vertexarray);
	ib.data(indices, indices_size);
}

void Mesh::update_diffuse_map(Texture diffusemap)
{
	this->diffusemap = diffusemap;
}

void Mesh::set_mode(GLenum mode)
{
	this->mode = mode;
}

void Mesh::Draw()
{
	glBindVertexArray(vertexarray); 

	diffusemap.loaded()	? diffusemap.bind(GL_TEXTURE0) : fgr::Texture::unbind(GL_TEXTURE0);
	normalmap.loaded()	? normalmap.bind(GL_TEXTURE1)  : fgr::Texture::unbind(GL_TEXTURE0);

	glDrawElements(mode, ib.get_count(), GL_UNSIGNED_INT, nullptr);
}
