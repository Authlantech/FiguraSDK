#pragma once

#include <Figura/mesh.h>
#include <Figura/Shader.h>
#include <glm/glm.hpp>
#include <thread>
#include <future>
#include <vector>

namespace fgr {

	struct model_data {
		std::vector<mesh_data> model_meshes;
	};

	class Model {
	protected :
		std::vector<std::shared_ptr<Mesh>> meshes;
		void Render();
	public:
		friend class GraphicsEngine;
		Model() = default;
		~Model() = default;
	protected :
		glm::mat4 translation = glm::mat4(1.f);
		glm::mat4 rotation = glm::mat4(1.f);
		glm::mat4 scaling = glm::mat4(1.f);
		glm::mat4 modelMatrix = glm::mat4(1.f);
		glm::mat4 normalMatrix = glm::mat4(1.f);
		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
	public : 
		void set_position(glm::vec3 position);
		void scale(float v);
		void rotate(glm::vec3 v, float angle);
		glm::vec3 get_position();
	protected : 
		void update_model_normal_matrices();
		glm::mat4 get_model_matrix();
		glm::mat4 get_normal_matrix();

		std::future<model_data> loaded_;
	public : 
		void Load(std::string file);
		void LoadAsync(std::string file);
		void Load(model_data data);
		static model_data LoadModelData(std::string file);
	};

	class Model2D : public Model
	{
	public : 
		Model2D(const char* texture_path);
	};

}