
#include <Figura/Figura.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/Importer.hpp>

namespace fgr
{
	GraphicsEngine graphic_engine;


	void GraphicsEngine::init_engine(const char* title,int width,int height)
	{
		// Create OpenGL Context
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width,height,title,0,0);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);
		// Init GLAD
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		//Begin Light Buffers 
		fgr::DirectionalLight::begin_directional_lights();
		fgr::PointLight::begin_point_lights();
		fgr::SpotLight::begin_spot_light();

		// Start and load shaders
		create_shader("phong shader", model_vs, model_fs);
		create_shader("normal shader", normal_vs, normal_fs,normal_gs); 
		create_shader("mesh shader", mesh_vs, mesh_fs, mesh_gs);
		
		use_shader("phong shader");

		//Enable Depth Testing and Face Culling : 
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

	}

	void GraphicsEngine::update_window() {
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 1, 0);
	}

	bool GraphicsEngine::window_is_open() {
		return !glfwWindowShouldClose(window);
	}

	void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransformation)
	{
		aiMatrix4x4 vertexTransformation = parentTransformation * node->mTransformation;
		aiMatrix3x3 normalTransformation = aiMatrix3x3(vertexTransformation).Inverse().Transpose();

		// Process the mesh :

		for (int a = 0; a < node->mNumMeshes; a++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[a]];

			for (int b = 0; b < mesh->mNumVertices; b++)
			{
				mesh->mVertices[b] = vertexTransformation * mesh->mVertices[b];
				mesh->mNormals[b]  = normalTransformation * mesh->mNormals[b];
				mesh->mTangents[b] = normalTransformation * mesh->mTangents[b];
				mesh->mBitangents[b] = normalTransformation * mesh->mBitangents[b];
			}
		}

		// Process the child nodes :

		for (int a = 0; a < node->mNumChildren; a++)
		{
			processNode(node->mChildren[a], scene, vertexTransformation);
		}

	}

	std::shared_ptr<fgr::Model> GraphicsEngine::create_and_load_model(std::string name, std::string path)
	{		
		std::shared_ptr<fgr::Model> model(new fgr::Model);

		auto load_func = [this](std::string p,std::shared_ptr<Model>m) -> void {

		};

		// Convert path to universal format
			std::string fpath = path;
			for (char& c : fpath)
				if (c == '\\') c = '/';

			std::string folder = fpath.substr(0, fpath.find_last_of("/"));
			std::string file_name = fpath.substr(fpath.find_last_of("/") + 1);

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(fpath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				std::cerr << importer.GetErrorString() << std::endl;
			}

			std::unordered_set<std::string> detected_textures;
			std::unordered_map<std::string,std::shared_ptr<Texture>> loaded_textures;

			aiTextureType desired_types[] = {
				aiTextureType_BASE_COLOR,
				aiTextureType_DIFFUSE,
				aiTextureType_NORMALS,
				aiTextureType_METALNESS,
				aiTextureType_DIFFUSE_ROUGHNESS,
				aiTextureType_AMBIENT_OCCLUSION
			};

			// Detect all textures

			for (unsigned int a = 0; a < scene->mNumMaterials; a++) {
				aiMaterial* material = scene->mMaterials[a];
				for (aiTextureType type : desired_types) {
					int texture_count = material->GetTextureCount(type);
					if (texture_count != 0) {
						aiString texture_path;
						material->GetTexture(type,0,&texture_path);
						std::string texture_file_name = std::string(texture_path.C_Str());
						if (file_name.find('\\') != std::string::npos) texture_file_name = texture_file_name.substr(texture_file_name.find_last_of('\\') + 1);
						else if (texture_file_name.find('/') != std::string::npos) texture_file_name = texture_file_name.substr(texture_file_name.find_last_of('/') + 1);
						detected_textures.insert(texture_file_name);
					}
				}
			}

			// Load all textures
			for (auto n : detected_textures) {
				std::shared_ptr<Texture> texture(new Texture);
				if (texture->LoadFromFile(std::string(folder + '/' + n).c_str())) {
					loaded_textures[n] = texture;
				}
			}

			// Loading all meshes
			processNode(scene->mRootNode, scene, aiMatrix4x4());

			for (unsigned int a = 0; a < scene->mNumMeshes; a++)
			{
				std::shared_ptr<Mesh> mesh(new Mesh);
				aiMesh* currentMesh = scene->mMeshes[a];

				std::vector<Vertex> vertices;
				for (unsigned int b = 0; b < currentMesh->mNumVertices; b++)
				{
					Vertex v;
					v.position[0] = currentMesh->mVertices[b].x;
					v.position[1] = currentMesh->mVertices[b].y;
					v.position[2] = currentMesh->mVertices[b].z;

					if (currentMesh->HasTextureCoords(0))
					{
						v.texture_coordinates[0] = currentMesh->mTextureCoords[0][b].x;
						v.texture_coordinates[1] = currentMesh->mTextureCoords[0][b].y;
					}
					if (currentMesh->HasNormals())
					{
						v.normal[0] = currentMesh->mNormals[b].x;
						v.normal[1] = currentMesh->mNormals[b].y;
						v.normal[2] = currentMesh->mNormals[b].z;
					}
					if (currentMesh->HasTangentsAndBitangents())
					{
						v.tangent[0] = currentMesh->mTangents[b].x;
						v.tangent[1] = currentMesh->mTangents[b].y;
						v.tangent[2] = currentMesh->mTangents[b].z;

						v.bittangent[0] = currentMesh->mBitangents[b].x;
						v.bittangent[1] = currentMesh->mBitangents[b].y;
						v.bittangent[2] = currentMesh->mBitangents[b].z;
					}
					vertices.push_back(v);
				}

				std::vector<unsigned int> indices;
				for (unsigned int b = 0; b < currentMesh->mNumFaces; b++)
				{
					for (unsigned int c = 0; c < currentMesh->mFaces[b].mNumIndices; c++)
					{
						indices.push_back(currentMesh->mFaces[b].mIndices[c]);
					}
				}

				std::vector<std::shared_ptr<Texture>> mesh_textures;
				aiMaterial* material = scene->mMaterials[currentMesh->mMaterialIndex];

				for (aiTextureType type : desired_types) {
					int texture_count = material->GetTextureCount(type);
					if (texture_count != 0) {
						aiString texture_path;
						material->GetTexture(type,0,&texture_path);
						std::string texture_file_name = std::string(texture_path.C_Str());
						if (texture_file_name.find('\\') != std::string::npos) texture_file_name = texture_file_name.substr(texture_file_name.find_last_of('\\') + 1);
						else if (texture_file_name.find('/') != std::string::npos) texture_file_name = texture_file_name.substr(texture_file_name.find_last_of('/') + 1);

						if (loaded_textures.find(texture_file_name) != loaded_textures.end()) {
							mesh_textures.push_back(loaded_textures[texture_file_name]);
						}
					}
				}

				mesh->Load(vertices, indices,mesh_textures);
				model->meshes.push_back(mesh);
			}

		models[name] = model;
		return model;
	}

	void GraphicsEngine::delete_model(std::string name)
	{
		try
		{
			if (models.at(name).use_count() > 1)
			{
				std::cout << "[Figura::Error] : At GraphicsEngine::delete_model, attempted to delete a shared object!\n";
				return;
			}

			else
			{
				models.erase(name);
			}
		}

		catch (std::exception& e)
		{
			std::cout << "[Figura::Exception] : \"" << e.what() << "\" thrown at GraphicsEngine::delete_model" << std::endl;
			return;
		}
	}

	std::shared_ptr<fgr::Model> GraphicsEngine::get_model(std::string name)
	{
		return models.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_model_names()
	{
		std::vector<std::string> names;
		for (auto& e : models) names.push_back(e.first);
		return names;
	}

	std::shared_ptr<fgr::Shader> GraphicsEngine::create_shader(std::string name, const char* vertex_shader_source, const char* fragment_shader_source, const char* geometry_shader_source)
	{
		std::shared_ptr<fgr::Shader> s(new fgr::Shader); 
		s->load_from_buffer(vertex_shader_source, fragment_shader_source, geometry_shader_source);
		shaders[name] = s;
		return s;
	}

	void GraphicsEngine::delete_shader(std::string name)
	{
		try
		{
			if (shaders.at(name).use_count() > 1)
			{
				std::cout << "[Figura::Error] : At GraphicsEngine::delete_shader, attempted to delete a shared object!\n";
				return;
			}

			else
			{
				shaders.erase(name);
			}
		}
		
		catch (std::exception& e)
		{
			std::cout << "[Figura::Exception] : \"" << e.what() << "\" thrown at GraphicsEngine::delete_shader" << std::endl; 
			return;
		}
	}

	void GraphicsEngine::use_shader(std::string name)
	{
		try
		{
			current_shader = shaders.at(name);
			current_shader->use();
		}

		catch (std::exception&e)
		{
			std::cout << "[Figura::Exception] : \"" << e.what() << "\" thrown at GraphicsEngine::use_shader" << std::endl;
			return;
		}
	}

	const std::vector<std::string> GraphicsEngine::get_shader_names()
	{
		std::vector<std::string> names; 
		for (auto& e : shaders) names.push_back(e.first);
		return names;
	}

	std::shared_ptr<fgr::Camera> GraphicsEngine::create_perspective_camera(std::string name, float fov, float aspect, float zNear, float zFar)
	{
		std::shared_ptr<fgr::Camera> c(new fgr::Camera); 
		c->create_perspective(fov, aspect, zNear, zFar);
		cameras[name] = c;
		return c;
	}

	std::shared_ptr<fgr::Camera> GraphicsEngine::create_orthographic_camera(std::string name, float left, float right, float bottom, float top, float near, float far)
	{
		std::shared_ptr<fgr::Camera> c(new fgr::Camera);
		c->create_ortho(left,right,bottom,top, near, far);
		cameras[name] = c;
		return c;
	}

	void GraphicsEngine::delete_camera(std::string name)
	{
		cameras.erase(name);
	}

	std::shared_ptr<fgr::Camera> GraphicsEngine::get_camera(std::string name)
	{
		return cameras.at(name); 
	}

	const std::vector<std::string> GraphicsEngine::get_camera_names()
	{
		std::vector<std::string> names; 
		for (auto& e : cameras) names.push_back(e.first); 
		return names;
	}

	std::shared_ptr<fgr::DirectionalLight> GraphicsEngine::create_directional_light(std::string name, glm::vec3 direction, glm::vec3 color)
	{
		std::shared_ptr<fgr::DirectionalLight> d_l(new fgr::DirectionalLight(direction,color)); 
		d_lights[name] = d_l;
		return d_l;
	}

	void GraphicsEngine::delete_directional_light(std::string name)
	{
		d_lights.erase(name);
	}

	std::shared_ptr<fgr::DirectionalLight> GraphicsEngine::get_directional_light(std::string name)
	{
		return d_lights.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_directional_light_names()
	{
		std::vector<std::string> names; 
		for (auto& e : d_lights) names.push_back(e.first); 
		return names;
	}

	std::shared_ptr<fgr::PointLight> GraphicsEngine::create_point_light(std::string name, glm::vec3 position, glm::vec3 color)
	{
		std::shared_ptr<fgr::PointLight> p_l(new fgr::PointLight(position,color)); 
		p_lights[name] = p_l;
		return p_l;
	}

	void GraphicsEngine::delete_point_light(std::string name)
	{
		p_lights.erase(name);
	}

	std::shared_ptr<fgr::PointLight> GraphicsEngine::get_point_light(std::string name)
	{
		return p_lights.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_point_light_names()
	{
		std::vector<std::string> names; 
		for (auto& e : p_lights) names.push_back(e.first); 
		return names;
	}

	std::shared_ptr<fgr::SpotLight> GraphicsEngine::create_spot_light(std::string name, glm::vec3 position, glm::vec3 color, glm::vec3 orientation, float angle)
	{
		std::shared_ptr<fgr::SpotLight> s_l(new fgr::SpotLight(position, color, orientation, angle)); 
		s_lights[name] = s_l;
		return s_l;
	}

	void GraphicsEngine::delete_spot_light(std::string name)
	{
		s_lights.erase(name);
	}

	std::shared_ptr<fgr::SpotLight> GraphicsEngine::get_spot_light(std::string name)
	{
		return s_lights.at(name);
	}

	const std::vector<std::string> GraphicsEngine::get_spot_light_names()
	{
		std::vector<std::string> names; 
		for (auto& e : s_lights) names.push_back(e.first); 
		return names;
	}

}
