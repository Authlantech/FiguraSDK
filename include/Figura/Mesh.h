#pragma once

#include <glad/glad.h>
#include <Figura/texture.h>
#include <vector>
#include <string>

namespace fgr {

	struct Vertex
	{
		float position[3];
		float color[4];
		float texture_coordinates[3];
		float normal[3];
		float tangent[3]; 
		float bittangent[3];
	};

	struct mesh_data {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		TEXTURE_DATA albedo;
		TEXTURE_DATA normal;
		TEXTURE_DATA metallic;
		TEXTURE_DATA roughness;
		TEXTURE_DATA ao;
	};

	class Mesh
	{
	protected:
		unsigned int vertexarray = 0;
		unsigned int vertexbuffer = 0;
		unsigned int indexbuffer = 0;
		unsigned int indices_count = 0;
		std::shared_ptr<Texture> albedo = nullptr;
		std::shared_ptr<Texture> normal = nullptr;
		std::shared_ptr<Texture> metallic = nullptr;
		std::shared_ptr<Texture> roughness = nullptr;
		std::shared_ptr<Texture> ao = nullptr;
	public:
		friend class Model;
		Mesh();
		~Mesh();
		void LoadFromData(mesh_data data);
		static mesh_data LoadMeshData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
			const char* albedo_path, 
			const char* normal_path = nullptr, 
			const char* metallic_path = nullptr,
			const char* roughness_path = nullptr, 
			const char* ao_path = nullptr);
	protected:
		void Render() const;
		
		void vb_bind() const;
		void vb_data(const Vertex* vertices, unsigned int size) const;
		void vb_set_layout() const;
		void ib_bind() const;
		void ib_data(const unsigned int* indices, unsigned int size);
	};

}