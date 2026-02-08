#pragma once

#include <Figura/vertexbuffer.h>
#include <Figura/indexbuffer.h>
#include <Figura/texture.h>
#include <vector>
#include <string>

namespace fgr {

	struct MESH_DATA {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<TEXTURE_DATA> textures;
	};

	class Mesh
	{
	protected:
		unsigned int vertexarray = 0;
		VertexBuffer vb;
		IndexBuffer ib;
		std::vector<std::shared_ptr<Texture>> textures;
	public:
		friend class Model;
		Mesh();
		~Mesh();
	protected:
		static MESH_DATA CreateMeshData(std::vector<Vertex> vertices,std::vector<unsigned int> indices,std::vector<std::string>texture_file_paths);
		void LoadFromData(MESH_DATA data);
		void Render() const;
	};

}