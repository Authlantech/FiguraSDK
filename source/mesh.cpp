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
	Texture diffusemap
	)
{
	glBindVertexArray(vertexarray); 
	vb.data(vertices, vertices_size); 
	ib.data(indices, indices_size); 

	_diffusemap = diffusemap;

	this->vertices.clear();
	this->indices.clear();
	 
	for (int a = 0; a < vertices_size / sizeof(fgr::vertex); a++)
	{
		this->vertices.push_back(vertices[a]);
	}

	for (int a = 0; a < indices_size / sizeof(unsigned int); a++)
	{
		this->indices.push_back(indices[a]);
	}

}

std::vector<fgr::vertex> Mesh::get_vertices()
{
	return vertices;
}

std::vector<unsigned int> Mesh::get_indices()
{
	return indices;
}

void Mesh::set_mode(GLenum mode)
{
	this->mode = mode;
}

void Mesh::Draw(Shader shader)
{
	glBindVertexArray(vertexarray); 
	if (_diffusemap.loaded()) {
		_diffusemap.bind(GL_TEXTURE0);
	}
	else {
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, 0);
	}	
	shader.uniformint("dmloaded", _diffusemap.loaded()); 
	glDrawElements(mode, ib.get_count(), GL_UNSIGNED_INT, nullptr);
}
