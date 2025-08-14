#include <Figura/mesh.h>

using namespace fgr;

void Mesh::generate()
{
	if (verticies.size() > 0 && vertexarray == 0)
	{
		glGenVertexArrays(1, &vertexarray); 
		glBindVertexArray(vertexarray); 

		vb.generate(); 
		vb.set_layout();
		vb.data(verticies.data(), verticies.size() * sizeof(fgr::vertex)); 

		ib.generate(); 
		ib.bind();
		ib.data(indicies.data(), indicies.size() * sizeof(unsigned int));
		
		diffusemap.generate(); 
		normalmap.generate();		

		verticies.clear(); 
		indicies.clear(); 				
	}
}

void Mesh::reset()
{	
	if (vertexarray != 0)
	{
		vb.destroy();
		ib.destroy();
		glDeleteVertexArrays(1, &vertexarray);
		vertexarray = 0;
	}

	diffusemap.reset();
	normalmap.reset();

	verticies.clear(); 
	indicies.clear();
}

void Mesh::load(
	std::vector<fgr::vertex> verticies,
	std::vector<unsigned int> indicies,
	Texture diffusemap,
	Texture normalmap
)
{
	this->reset();

	this->diffusemap = diffusemap; 
	this->normalmap = normalmap;

	this->verticies = verticies; 
	this->indicies = indicies; 
}

void Mesh::Draw()
{	
	generate();

	if (vertexarray != 0 && verticies.empty())
	{
		glBindVertexArray(vertexarray);
		diffusemap.bind(GL_TEXTURE0);
		normalmap.bind(GL_TEXTURE1);
		
		glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);		
	}
}

