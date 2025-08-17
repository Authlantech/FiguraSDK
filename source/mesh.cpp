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

	albedo_map_name.clear(); 
	normal_map_name.clear();
	metalness_map_name.clear();
	roughness_map_name.clear();
	ao_map_name.clear();

	verticies.clear(); 
	indicies.clear();
}

void Mesh::load(
	std::vector<fgr::vertex> verticies,
	std::vector<unsigned int> indicies,
	std::string albedo_map_name,
	std::string normal_map_name,
	std::string metalnessmapname,
	std::string roughnessmapname,
	std::string aomapname
)
{
	this->reset();

	this->verticies = verticies; 
	this->indicies = indicies; 

	this->albedo_map_name = albedo_map_name; 
	this->normal_map_name = normal_map_name;
	this->metalness_map_name = metalnessmapname;
	this->roughness_map_name = roughnessmapname;
	this->ao_map_name = aomapname;
}

void Mesh::Render()
{	
	generate();

	if (vertexarray != 0)
	{
		glBindVertexArray(vertexarray);
		glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);		
	}
}

