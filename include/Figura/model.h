#pragma once 
#include <Figura/mesh.h>
#include <Figura/texture.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <exception>
#include <queue>
#include <thread>
#include <future>
#include <vector>
#include <unordered_map>
#include <string> 

namespace fgr {

	class Model {

	private:
		std::vector<std::unique_ptr<fgr::Mesh>> meshes;
		std::unordered_map<std::string, std::unique_ptr<fgr::Texture>>	albedo_maps;
		std::unordered_map<std::string, std::unique_ptr<fgr::Texture>>	normal_maps;
		std::unordered_map<std::string, std::unique_ptr<fgr::Texture>>	metalness_maps;
		std::unordered_map<std::string, std::unique_ptr<fgr::Texture>>	roughness_maps;
		std::unordered_map<std::string, std::unique_ptr<fgr::Texture>>	ao_maps;

		glm::mat4 translation = glm::mat4(1.f);
		glm::mat4 rotation = glm::mat4(1.f);
		glm::mat4 scaling = glm::mat4(1.f);
		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
		std::future<void> loading_thread_checker;

		static std::queue < std::tuple<std::shared_ptr<fgr::Model>, const char*, std::promise<void>* >> pending_loads;
		static void model_loading_thread();
		static void load_model(std::shared_ptr<fgr::Model> model, const char* path, std::promise<void>* p);
	public:
		Model() {};
		~Model() {}; 
		friend class GraphicsEngine;

		void set_position(glm::vec3 position);
		void scale(float v);
		void rotate(glm::vec3 v, float angle);

		glm::vec3 get_position();

		void Render();
	};

}