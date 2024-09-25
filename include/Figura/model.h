#pragma once 
#include "mesh.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream> 
#include <thread>
#include <vector>
#include <map>
#include <string>

namespace fgr {

	class Model {
	private:
		std::vector<fgr::Mesh> meshes;
		glm::mat4 translation = glm::mat4(1.f);
		glm::mat4 rotation = glm::mat4(1.f);
		glm::mat4 scaling = glm::mat4(1.f);

		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);

	public:

		void add_mesh(Mesh mesh);
		void set_position(glm::vec3 position);
		void scale(float v);
		void rotate(glm::vec3 v, float angle);

		glm::vec3 get_position();
		glm::mat4 get_normalMatrix(); 
		glm::mat4 get_modelMatrix();

		std::vector<fgr::Mesh> get_meshes();

		bool Load(const char* path);
		void Draw(Shader shader);
	};

}