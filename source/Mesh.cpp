#include <Figura/mesh.h>

using namespace fgr;

Mesh::Mesh(){
	glGenVertexArrays(1,&vertexarray);
	glBindVertexArray(vertexarray);
	
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);
	
	vb_set_layout();
	ib_bind();
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1,&vertexarray);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &indexbuffer);
}

void Mesh::LoadFromData(mesh_data data) {
	glBindVertexArray(vertexarray);

	vb_data(data.vertices.data(), data.vertices.size() * sizeof(Vertex));
	ib_data(data.indices.data(), data.indices.size() * sizeof(unsigned int));

	// Albedo is mandatory
	albedo = std::make_shared<Texture>();
	albedo->LoadFromData(data.albedo);

	// Optional textures - only create if data is provided
	if (data.normal.pixels) {
		normal = std::make_shared<Texture>();
		normal->LoadFromData(data.normal);
	}

	if (data.metallic.pixels) {
		metallic = std::make_shared<Texture>();
		metallic->LoadFromData(data.metallic);
	}

	if (data.roughness.pixels) {
		roughness = std::make_shared<Texture>();
		roughness->LoadFromData(data.roughness);
	}

	if (data.ao.pixels) {
		ao = std::make_shared<Texture>();
		ao->LoadFromData(data.ao);
	}
}

void Mesh::Render() const
{
	glBindVertexArray(vertexarray);
	if (albedo) albedo->bind(GL_TEXTURE0);
	if (normal) normal->bind(GL_TEXTURE1);
	if (metallic) metallic->bind(GL_TEXTURE2);
	if (roughness) roughness->bind(GL_TEXTURE3);
	if (ao) ao->bind(GL_TEXTURE4);
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
}

mesh_data Mesh::LoadMeshData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
	const char* albedo_path, const char* normal_path, 
	const char* metallic_path, const char* roughness_path, 
	const char* ao_path) {
	mesh_data data;
	data.vertices = vertices;
	data.indices = indices;
	data.albedo = Texture::LoadTextureData(albedo_path);
	if (normal_path) data.normal = Texture::LoadTextureData(normal_path);
	if (metallic_path) data.metallic = Texture::LoadTextureData(metallic_path);
	if (roughness_path) data.roughness = Texture::LoadTextureData(roughness_path);
	if (ao_path) data.ao = Texture::LoadTextureData(ao_path);
	return data;
}

// Vertex buffer methods
void Mesh::vb_bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
}

void Mesh::vb_data(const Vertex* vertices, unsigned int size) const
{
	vb_bind();
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void Mesh::vb_set_layout() const
{
	vb_bind();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)0);					// float position[3]
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(float) * 3));	// float color[4]
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(float) * 7));	// float texture_coordinates[3]
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(float) * 10));	// float normal[3]
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(float) * 13));	// float tangent[3]
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(float) * 16));	// float bittangent[3]
}

// Index buffer methods
void Mesh::ib_bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
}

void Mesh::ib_data(const unsigned int* indices, unsigned int size)
{
	ib_bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	indices_count = size / sizeof(unsigned int);
}