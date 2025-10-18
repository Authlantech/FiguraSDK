#include <Figura/mesh.h>

using namespace fgr;

Mesh::Mesh(){
	glGenVertexArrays(1,&vertexarray);
	glBindVertexArray(vertexarray);
	vb.set_layout();
	ib.bind();
}

Mesh::Mesh(const std::vector<Vertex> vertices_, const std::vector<unsigned int> indices_, const std::vector<std::shared_ptr<Texture>> textures_) {
	glGenVertexArrays(1,&vertexarray);
	glBindVertexArray(vertexarray);
	vb.set_layout();
	ib.bind();
	Load(vertices_, indices_, textures_);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1,&vertexarray);
}

void Mesh::Load(const std::vector<Vertex> vertices_, const std::vector<unsigned int> indices_, const std::vector<std::shared_ptr<Texture>> textures_)
{
	glBindVertexArray(vertexarray);

	vb.data(vertices_.data(), vertices_.size() * sizeof(Vertex));
	ib.data(indices_.data(), indices_.size() * sizeof(unsigned int));

	textures = textures_;
}

void Mesh::Render() const
{
	glBindVertexArray(vertexarray);
	for (int t = 0;t < textures.size();t++) {
		textures[t]->bind(GL_TEXTURE0 + t);
	}
	glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);
}

