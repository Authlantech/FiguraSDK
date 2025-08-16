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

	diffuse_map_name.clear(); 
	normal_map_name.clear();

	verticies.clear(); 
	indicies.clear();
}

void Mesh::load(
	std::vector<fgr::vertex> verticies,
	std::vector<unsigned int> indicies,
	std::string diffuse_map_name,
	std::string normal_map_name
)
{
	this->reset();

	this->verticies = verticies; 
	this->indicies = indicies; 

	this->diffuse_map_name = diffuse_map_name; 
	this->normal_map_name = normal_map_name;
}

void Mesh::Draw()
{	
	generate();

	if (vertexarray != 0)
	{
		glBindVertexArray(vertexarray);
		glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);		
	}
}

