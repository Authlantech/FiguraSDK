#pragma once

#include <Figura/mesh.h>
#include <Figura/Shader.h>
#include <glm/glm.hpp>
#include <thread>
#include <future>
#include <vector>

namespace fgr {

	struct model_data {
		std::vector<MESH_DATA> model_meshes;
	};

	model_data LoadModelData(std::string file);

	class Model {
	protected :
		std::vector<std::shared_ptr<Mesh>> meshes;

		glm::mat4 translation = glm::mat4(1.f);
		glm::mat4 rotation = glm::mat4(1.f);
		glm::mat4 scaling = glm::mat4(1.f);

		glm::mat4 modelMatrix = glm::mat4(1.f);
		glm::mat4 normalMatrix = glm::mat4(1.f);

		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);

		void updateModelMatrix();
		void updateNormalMatrix();

		std::future<model_data> is_loaded;
		void LoadFromData(model_data data);
		void Render();

	public:
		friend class GraphicsEngine;
		Model() = default;
		
		void set_position(glm::vec3 position);
		void scale(float v);
		void rotate(glm::vec3 v, float angle);

		glm::vec3 get_position();
		glm::mat4 get_modelMatrix();
		glm::mat4 get_normalMatrix();
	};

}