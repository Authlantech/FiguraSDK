#pragma once

#include <Figura/mesh.h>
#include <Figura/Shader.h>
#include <glm/glm.hpp>
#include <thread>
#include <vector>

namespace fgr {

	struct MODEL_DATA {
		std::vector<MESH_DATA> model_meshes;
	};

	class Model {
		std::vector<std::shared_ptr<Mesh>> meshes;
		glm::mat4 translation = glm::mat4(1.f);
		glm::mat4 rotation = glm::mat4(1.f);
		glm::mat4 scaling = glm::mat4(1.f);
		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
	public:
		friend class Scene;
		void set_position(glm::vec3 position);
		void scale(float v);
		void rotate(glm::vec3 v, float angle);
		glm::vec3 get_position();

		void LoadFromData(MODEL_DATA data);
		static MODEL_DATA LoadModelData(std::string path);
		static MODEL_DATA LoadModelData(std::vector<MESH_DATA> meshes);
	private :
		void Render(std::shared_ptr<Shader> shader);
	};

}