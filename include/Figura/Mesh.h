#pragma once

#include <Figura/vertexbuffer.h>
#include <Figura/indexbuffer.h>
#include <Figura/texture.h>
#include <vector>
#include <string>

namespace fgr {



	class Mesh
	{
	public :
		struct DATA {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture::DATA> textures;
	};
	private :
		unsigned int vertexarray = 0;
		VertexBuffer vb;
		IndexBuffer ib;
		std::vector<std::shared_ptr<Texture>> textures;
	public:
		friend class Model;
		Mesh();
		~Mesh();

		void LoadFromData(DATA data);
		static DATA CreateMeshData(std::vector<Vertex> vertices,std::vector<unsigned int> indices,std::vector<std::string>texture_file_paths);
		void Render() const;
	};

}