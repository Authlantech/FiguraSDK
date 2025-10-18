#pragma once

#include <Figura/vertexbuffer.h>
#include <Figura/indexbuffer.h>
#include <Figura/texture.h>
#include <vector>

namespace fgr {

	class Mesh
	{
		unsigned int vertexarray = 0;
		VertexBuffer vb;
		IndexBuffer ib;
		std::vector<std::shared_ptr<Texture>> textures;
	public:
		friend class Model;

		Mesh();
		Mesh(const std::vector<Vertex> vertices_, const std::vector<unsigned int> indices_, const std::vector<std::shared_ptr<Texture>> textures_);
		~Mesh();
		void Load(const std::vector<Vertex> vertices_, const std::vector<unsigned int> indices_, const std::vector<std::shared_ptr<Texture>> textures_);
		void Render() const;
	};

}