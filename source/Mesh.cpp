#include <Figura/mesh.h>

using namespace fgr;

Mesh::Mesh(){
	glGenVertexArrays(1,&vertexarray);
	glBindVertexArray(vertexarray);
	vb.set_layout();
	ib.bind();
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1,&vertexarray);
}

void Mesh::LoadFromData(MESH_DATA data) {
	glBindVertexArray(vertexarray);

	vb.data(data.vertices.data(),data.vertices.size() * sizeof(Vertex));
	ib.data(data.indices.data(),data.indices.size() * sizeof(unsigned int));

	textures.clear();
	for (auto& texture : data.textures) {
		std::shared_ptr<Texture> texture_ptr = std::make_shared<Texture>();
		texture_ptr->LoadFromData(texture);
		textures.push_back(texture_ptr);
	}
}

void Mesh::Render() const
{
	glBindVertexArray(vertexarray);
	for (int t = 0;t < textures.size();t++) {
		textures[t]->bind(GL_TEXTURE0 + t);
	}
	glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);
}

MESH_DATA Mesh::CreateMeshData(std::vector<Vertex> vertices,std::vector<unsigned int> indices,std::vector<std::string>texture_file_paths) {
	MESH_DATA data;
	data.vertices = vertices;
	data.indices = indices;
	for (const auto& file_path : texture_file_paths) {
		TEXTURE_DATA texture_data = Texture::LoadTextureData(file_path.c_str());
	}
	return data;
}