#pragma once

#include <Figura/mesh.h>
#include <glm/glm.hpp>
#include <thread>
#include <vector>

namespace fgr {

	class Model {
		std::vector<std::shared_ptr<Mesh>> meshes;
		glm::mat4 translation = glm::mat4(1.f);
		glm::mat4 rotation = glm::mat4(1.f);
		glm::mat4 scaling = glm::mat4(1.f);
		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
	public:
		friend class GraphicsEngine;
		void set_position(glm::vec3 position);
		void scale(float v);
		void rotate(glm::vec3 v, float angle);
		glm::vec3 get_position();
		void Load(std::vector<std::shared_ptr<Mesh>> meshes_);
		void Render();
	};

}